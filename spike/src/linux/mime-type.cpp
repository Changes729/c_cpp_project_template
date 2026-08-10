#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gio/gio.h>

// 存储应用信息（持有 GAppInfo 引用，方便后续启动）
typedef struct {
    GAppInfo *app_info;
    char     *name;
    char     *app_id;
    char     *executable;
} AppEntry;

// 判断 GError 是否表示“需要终端”类的错误
static gboolean error_needs_terminal(const GError *error)
{
    if (!error || !error->message) return FALSE;
    // GIO 对终端应用返回 "Unable to find terminal required for application"
    return strstr(error->message, "terminal") != NULL
        || strstr(error->message, "Terminal") != NULL;
}

// 在新终端窗口中启动应用并打开文件，完全脱离父进程
static gboolean launch_in_terminal(const char *executable,
                                   const char *file_path)
{
    // 按优先级查找可用的终端模拟器
    const char *terminals[] = {
        "gnome-terminal", "konsole", "xfce4-terminal",
        "terminator",     "lxterminal", "alacritty",
        "kitty",          "foot",       "xterm",
        NULL
    };

    const char *found_term = NULL;
    for (int i = 0; terminals[i]; i++) {
        char *p = g_find_program_in_path(terminals[i]);
        if (p) {
            found_term = terminals[i];
            g_free(p);
            break;
        }
    }
    if (!found_term) {
        fprintf(stderr, "未找到可用的终端模拟器\n");
        return FALSE;
    }

    // 根据终端类型构造启动命令
    char *cmd = NULL;
    if (g_strcmp0(found_term, "gnome-terminal") == 0) {
        cmd = g_strdup_printf("gnome-terminal -- %s \"%s\"",
                              executable, file_path);
    } else if (g_strcmp0(found_term, "terminator") == 0) {
        // terminator 用 -x，命令和参数分开传
        cmd = g_strdup_printf("terminator -x %s \"%s\"",
                              executable, file_path);
    } else if (g_strcmp0(found_term, "kitty") == 0
               || g_strcmp0(found_term, "foot") == 0) {
        cmd = g_strdup_printf("%s %s \"%s\"",
                              found_term, executable, file_path);
    } else if (g_strcmp0(found_term, "alacritty") == 0) {
        cmd = g_strdup_printf("alacritty -e %s \"%s\"",
                              executable, file_path);
    } else {
        // konsole / xfce4-terminal / lxterminal / xterm
        cmd = g_strdup_printf("%s -e %s \"%s\"",
                              found_term, executable, file_path);
    }

    printf("启动终端: %s\n", cmd);

    GError *error = NULL;
    // g_spawn_command_line_async 自动 fork + setsid，脱离父进程
    gboolean ok = g_spawn_command_line_async(cmd, &error);
    if (!ok) {
        fprintf(stderr, "终端启动失败: %s\n",
                error ? error->message : "未知错误");
        if (error) g_error_free(error);
    }
    g_free(cmd);
    return ok;
}

static void app_entry_free(AppEntry *entry) {
    if (!entry) return;
    if (entry->app_info) g_object_unref(entry->app_info);
    g_free(entry->name);
    g_free(entry->app_id);
    g_free(entry->executable);
    g_free(entry);
}

void print_app_info(GAppInfo *app, const char *prefix) {
    if (!app) return;

    const char *name = g_app_info_get_name(app);
    const char *executable = g_app_info_get_executable(app);
    const char *id = g_app_info_get_id(app);

    printf("%s: %s\n", prefix, name);
    printf("  - App ID : %s\n", id ? id : "N/A");
    printf("  - Exec   : %s\n", executable ? executable : "N/A");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("用法: %s <文件路径>\n", argv[0]);
        printf("示例: %s /etc/hosts\n", argv[0]);
        printf("示例: %s /home/user/example.png\n", argv[0]);
        return 1;
    }

    const char *path = argv[1];

    if (!g_file_test(path, G_FILE_TEST_EXISTS)) {
        fprintf(stderr, "文件不存在: %s\n", path);
        return 1;
    }

    if (!g_file_test(path, G_FILE_TEST_IS_REGULAR)
        && !g_file_test(path, G_FILE_TEST_IS_DIR)) {
        fprintf(stderr, "不是普通文件或目录: %s\n", path);
        return 1;
    }

    GFile *file = g_file_new_for_path(path);
    GError *error = NULL;
    GFileInfo *info = g_file_query_info(
        file,
        G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE,
        G_FILE_QUERY_INFO_NONE,
        NULL,
        &error);

    if (!info) {
        fprintf(stderr, "获取文件信息失败: %s\n", error ? error->message : "未知错误");
        if (error) {
            g_error_free(error);
        }
        g_object_unref(file);
        return 1;
    }

    const char *content_type = g_file_info_get_content_type(info);
    char *mime_type = g_content_type_get_mime_type(content_type ? content_type : "application/octet-stream");

    printf("=== 查询文件: %s ===\n", path);
    printf("文件类型: %s\n", content_type ? content_type : "N/A");
    printf("MIME 类型: %s\n\n", mime_type ? mime_type : "N/A");

    // --- 获取默认应用 ---
    GAppInfo *default_app = g_app_info_get_default_for_type(mime_type ? mime_type : content_type, FALSE);
    const char *default_id = NULL;
    if (default_app) {
        default_id = g_app_info_get_id(default_app);
    }

    // --- 获取所有可打开该文件的应用，存入数组 ---
    GList *app_list = g_app_info_get_all_for_type(mime_type ? mime_type : content_type);
    GPtrArray *apps = g_ptr_array_new();  // 存储 AppEntry*
    int default_index = -1;  // 默认应用在列表中的位置（1-based）

    if (app_list) {
        int count = 1;
        printf("【所有可打开该文件的应用列表】:\n");

        for (GList *l = app_list; l != NULL; l = l->next) {
            GAppInfo *app = G_APP_INFO(l->data);

            AppEntry *entry = g_new0(AppEntry, 1);
            entry->app_info   = g_object_ref(app);
            entry->name       = g_strdup(g_app_info_get_name(app));
            entry->app_id     = g_strdup(g_app_info_get_id(app));
            entry->executable = g_strdup(g_app_info_get_executable(app));

            g_ptr_array_add(apps, entry);

            // 检查是否为默认应用
            int is_default = (default_id && entry->app_id
                              && g_strcmp0(default_id, entry->app_id) == 0);
            if (is_default) {
                default_index = count;
            }

            printf("\n[%d]%s ", count, is_default ? " ★ (默认)" : "");
            count++;
            print_app_info(app, "应用");
        }
        printf("\n");

        // 如果默认应用不在列表中，追加到列表末尾
        if (default_index < 0 && default_app) {
            AppEntry *entry = g_new0(AppEntry, 1);
            entry->app_info   = g_object_ref(default_app);
            entry->name       = g_strdup(g_app_info_get_name(default_app));
            entry->app_id     = g_strdup(g_app_info_get_id(default_app));
            entry->executable = g_strdup(g_app_info_get_executable(default_app));

            g_ptr_array_add(apps, entry);
            default_index = apps->len;

            printf("[%d]%s ", default_index, " ★ (默认)");
            print_app_info(default_app, "应用");
            printf("\n");
        }

        g_list_free_full(app_list, g_object_unref);
    } else if (default_app) {
        // 没有任何注册应用，但存在默认应用
        printf("【所有可打开该文件的应用列表】:\n");

        AppEntry *entry = g_new0(AppEntry, 1);
        entry->app_info   = g_object_ref(default_app);
        entry->name       = g_strdup(g_app_info_get_name(default_app));
        entry->app_id     = g_strdup(g_app_info_get_id(default_app));
        entry->executable = g_strdup(g_app_info_get_executable(default_app));

        g_ptr_array_add(apps, entry);
        default_index = 1;

        printf("\n[1] ★ (默认) ");
        print_app_info(default_app, "应用");
        printf("\n");
    } else {
        printf("【所有可打开该文件的应用列表】: 未找到任何支持的应用\n");
    }

    if (default_app) {
        g_object_unref(default_app);
    }

    // --- 交互式选择并打开 ---
    if (apps->len > 0) {
        printf("----------------------------------------\n");
        printf("请输入序号选择要打开文件的应用 (输入 0 取消): ");

        int choice;
        if (scanf("%d", &choice) != 1) {
            choice = 0;
        }

        if (choice > 0 && choice <= (int)apps->len) {
            AppEntry *selected = (AppEntry *)g_ptr_array_index(apps, choice - 1);

            printf("\n正在使用 [%s] 打开文件...\n", selected->name);

            gboolean success = FALSE;

            // 先尝试 GIO launch
            GList *file_list = g_list_append(NULL, file);
            success = g_app_info_launch(selected->app_info,
                                         file_list, NULL, &error);
            g_list_free(file_list);

            if (!success && error_needs_terminal(error)) {
                // 应用需要终端（如 vim），在新窗口中启动
                fprintf(stderr, "GIO 需要终端，切换到终端启动模式\n");
                g_error_free(error);
                error = NULL;
                success = launch_in_terminal(selected->executable, path);
            } else if (!success) {
                fprintf(stderr, "GIO 启动失败: %s\n",
                        error ? error->message : "未知错误");
                if (error) { g_error_free(error); error = NULL; }

                // 备用方案：g_app_info_launch_uris
                char *uri = g_file_get_uri(file);
                GList *uri_list = g_list_append(NULL, uri);
                success = g_app_info_launch_uris(selected->app_info,
                                                  uri_list, NULL, &error);
                g_list_free(uri_list);
                g_free(uri);

                if (!success) {
                    if (error_needs_terminal(error)) {
                        fprintf(stderr, "URI 启动也需要终端，切换到终端启动\n");
                        g_error_free(error);
                        error = NULL;
                        success = launch_in_terminal(selected->executable, path);
                    } else {
                        fprintf(stderr, "URI 启动也失败: %s\n",
                                error ? error->message : "未知错误");
                        if (error) { g_error_free(error); error = NULL; }
                    }
                }
            }

            if (success) {
                printf("文件已成功打开!\n");
            } else {
                // 最终备用方案：xdg-open
                char *cmd = g_strdup_printf("xdg-open \"%s\" &", path);
                printf("尝试最终备用方案: %s\n", cmd);
                int ret = system(cmd);
                g_free(cmd);
                if (ret == 0) {
                    printf("文件已成功打开!（xdg-open）\n");
                } else {
                    fprintf(stderr, "所有方案均失败。\n");
                }
            }
        } else if (choice == 0) {
            printf("已取消。\n");
        } else {
            fprintf(stderr, "无效的选择!\n");
        }
    }

    // 清理
    g_ptr_array_free(apps, TRUE);  // TRUE = 同时释放元素
    g_free(mime_type);
    g_object_unref(info);
    g_object_unref(file);

    return 0;
}