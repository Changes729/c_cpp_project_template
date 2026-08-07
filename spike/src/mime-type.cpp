#include <stdio.h>
#include <gio/gio.h>

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

    if (!g_file_test(path, G_FILE_TEST_IS_REGULAR)) {
        fprintf(stderr, "不是普通文件: %s\n", path);
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

    GAppInfo *default_app = g_app_info_get_default_for_type(mime_type ? mime_type : content_type, FALSE);
    if (default_app) {
        print_app_info(default_app, "【默认打开应用】");
        g_object_unref(default_app);
    } else {
        printf("【默认打开应用】: 未找到匹配的默认应用\n");
    }

    printf("\n");

    GList *app_list = g_app_info_get_all_for_type(mime_type ? mime_type : content_type);
    if (app_list) {
        printf("【所有可打开该文件的应用列表】:\n");
        int count = 1;
        for (GList *l = app_list; l != NULL; l = l->next) {
            GAppInfo *app = (GAppInfo *)l->data;
            printf("\n[%d] ", count++);
            print_app_info(app, "应用");
        }
        g_list_free_full(app_list, g_object_unref);
    } else {
        printf("【所有可打开该文件的应用列表】: 未找到任何支持的应用\n");
    }

    g_free(mime_type);
    g_object_unref(info);
    g_object_unref(file);

    return 0;
}