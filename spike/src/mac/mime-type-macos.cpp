#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>

#include <iostream>
#include <string>
#include <vector>

struct AppInfo
{
  std::string name;
  std::string bundleId;
  std::string path;
  CFURLRef    appURL;
};

// 从 CFURL 获取应用的显示名称和 Bundle ID
AppInfo getAppInfo(CFURLRef appURL)
{
  AppInfo info;
  info.appURL   = appURL;
  info.path     = "N/A";
  info.name     = "N/A";
  info.bundleId = "N/A";

  // 获取应用路径
  CFStringRef pathStr = CFURLCopyFileSystemPath(appURL, kCFURLPOSIXPathStyle);
  if(pathStr) {
    char buf[PATH_MAX];
    if(CFStringGetCString(pathStr, buf, sizeof(buf), kCFStringEncodingUTF8)) {
      info.path = buf;
    }
    CFRelease(pathStr);
  }

  // 获取 Bundle
  CFBundleRef bundle = CFBundleCreate(NULL, appURL);
  if(bundle) {
    // Bundle display name
    CFStringRef displayName = (CFStringRef)
        CFBundleGetValueForInfoDictionaryKey(bundle,
                                             CFSTR("CFBundleDisplayName"));
    if(!displayName) {
      displayName = (CFStringRef)
          CFBundleGetValueForInfoDictionaryKey(bundle, CFSTR("CFBundleName"));
    }
    if(!displayName) {
      // 从路径中提取应用名
      CFStringRef appName = CFURLCopyLastPathComponent(appURL);
      if(appName) {
        displayName = appName;
      }
    }
    if(displayName) {
      char buf[PATH_MAX];
      if(CFStringGetCString(displayName, buf, sizeof(buf), kCFStringEncodingUTF8))
      {
        info.name = buf;
      }
    }

    // Bundle ID
    CFStringRef bundleId = CFBundleGetIdentifier(bundle);
    if(bundleId) {
      char buf[PATH_MAX];
      if(CFStringGetCString(bundleId, buf, sizeof(buf), kCFStringEncodingUTF8)) {
        info.bundleId = buf;
      }
    }

    CFRelease(bundle);
  }

  // 如果没有拿到名字，用路径最后一段
  if(info.name == "N/A") {
    CFStringRef appName = CFURLCopyLastPathComponent(appURL);
    if(appName) {
      char buf[PATH_MAX];
      if(CFStringGetCString(appName, buf, sizeof(buf), kCFStringEncodingUTF8)) {
        info.name = buf;
      }
      CFRelease(appName);
    }
  }

  return info;
}

int main(int argc, char *argv[])
{
  if(argc < 2) {
    std::cout << "用法: " << argv[0] << " <文件路径>" << std::endl;
    std::cout << "示例: " << argv[0] << " /etc/hosts" << std::endl;
    std::cout << "示例: " << argv[0] << " ~/Desktop/example.png" << std::endl;
    return 1;
  }

  const char *filePath = argv[1];

  // 展开 ~ 路径
  CFStringRef pathStr =
      CFStringCreateWithCString(NULL, filePath, kCFStringEncodingUTF8);
  CFStringRef expandedPath = CFStringCreateMutableCopy(NULL, 0, pathStr);
  CFRelease(pathStr);

  if(!expandedPath) {
    std::cerr << "路径解析失败: " << filePath << std::endl;
    return 1;
  }

  CFURLRef fileURL = CFURLCreateWithFileSystemPath(NULL,
                                                   expandedPath,
                                                   kCFURLPOSIXPathStyle,
                                                   false);
  CFRelease(expandedPath);

  if(!fileURL) {
    std::cerr << "无法创建文件 URL: " << filePath << std::endl;
    return 1;
  }

  // 检查文件是否存在
  if(!CFURLResourceIsReachable(fileURL, NULL)) {
    std::cerr << "文件不存在或无法访问: " << filePath << std::endl;
    CFRelease(fileURL);
    return 1;
  }

  std::cout << "=== 查询文件: " << filePath << " ===" << std::endl << std::endl;

  // --- 获取默认应用 ---
  CFErrorRef error = NULL;
  CFURLRef   defaultAppURL =
      LSCopyDefaultApplicationURLForURL(fileURL,
                                        kLSRolesAll,  // 所有角色（Viewer + Editor）
                                        &error);

  if(defaultAppURL) {
    AppInfo defaultApp = getAppInfo(defaultAppURL);
    std::cout << "【默认打开应用】" << std::endl;
    std::cout << "  名称    : " << defaultApp.name << std::endl;
    std::cout << "  Bundle ID: " << defaultApp.bundleId << std::endl;
    std::cout << "  路径    : " << defaultApp.path << std::endl;
    CFRelease(defaultAppURL);
  } else {
    std::cout << "【默认打开应用】: 未找到" << std::endl;
    if(error) {
      CFStringRef errDesc = CFErrorCopyDescription(error);
      if(errDesc) {
        char buf[1024];
        if(CFStringGetCString(errDesc, buf, sizeof(buf), kCFStringEncodingUTF8))
        {
          std::cerr << "  错误: " << buf << std::endl;
        }
        CFRelease(errDesc);
      }
      CFRelease(error);
    }
  }

  std::cout << std::endl;

  // --- 获取所有可打开该文件的应用 ---
  CFArrayRef appURLs =
      LSCopyApplicationURLsForURL(fileURL,
                                  kLSRolesAll);  // Viewer + Editor 角色

  std::vector<AppInfo> apps;

  if(appURLs) {
    CFIndex count = CFArrayGetCount(appURLs);

    if(count == 0) {
      std::cout << "【所有可打开该文件的应用】: 未找到任何支持的应用"
                << std::endl;
    } else {
      std::cout << "【所有可打开该文件的应用列表】 (" << count
                << " 个):" << std::endl
                << std::endl;

      for(CFIndex i = 0; i < count; i++) {
        CFURLRef appURL = (CFURLRef)CFArrayGetValueAtIndex(appURLs, i);
        AppInfo  info   = getAppInfo(appURL);
        apps.push_back(info);

        std::cout << "[" << (i + 1) << "] " << info.name << std::endl;
        std::cout << "    Bundle ID: " << info.bundleId << std::endl;
        std::cout << "    路径: " << info.path << std::endl;
        std::cout << std::endl;
      }
    }
  } else {
    std::cout << "【所有可打开该文件的应用】: 未找到任何支持的应用" << std::endl;
  }

  // --- 交互式选择并打开 ---
  if(!apps.empty()) {
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "请输入序号选择要打开文件的应用 (输入 0 取消): ";

    int choice;
    std::cin >> choice;

    if(choice > 0 && choice <= (int)apps.size()) {
      AppInfo &selected = apps[choice - 1];

      std::cout << std::endl;
      std::cout << "正在使用 [" << selected.name << "] 打开文件..." << std::endl;

      // 使用 LSOpenURLsWithRole 用指定应用打开文件
      CFArrayRef fileURLs =
          CFArrayCreate(NULL, (const void **)&fileURL, 1, &kCFTypeArrayCallBacks);
      LSLaunchURLSpec launchSpec = {0};
      launchSpec.appURL          = selected.appURL;
      launchSpec.itemURLs        = fileURLs;
      launchSpec.launchFlags     = kLSLaunchDefaults;
      launchSpec.asyncRefCon     = NULL;

      OSStatus status = LSOpenFromURLSpec(&launchSpec, NULL);

      CFRelease(fileURLs);

      if(status == noErr) {
        std::cout << "文件已成功打开!" << std::endl;
      } else {
        std::cerr << "打开文件失败，错误码: " << status << std::endl;

        // 备用方案：使用 open 命令
        std::string cmd =
            "open -a \"" + selected.path + "\" \"" + filePath + "\"";
        std::cout << "尝试使用备用方案: " << cmd << std::endl;
        int ret = system(cmd.c_str());
        if(ret == 0) {
          std::cout << "文件已成功打开!" << std::endl;
        } else {
          std::cerr << "备用方案也失败了。" << std::endl;
        }
      }
    } else if(choice == 0) {
      std::cout << "已取消。" << std::endl;
    } else {
      std::cerr << "无效的选择!" << std::endl;
    }
  }

  if(appURLs) {
    CFRelease(appURLs);
  }
  CFRelease(fileURL);

  return 0;
}
