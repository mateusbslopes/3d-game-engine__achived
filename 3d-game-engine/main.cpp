#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include <vector>
#include <cstring>

class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    // uint32_t for portability https://stackoverflow.com/questions/20077313/uint32-t-vs-int-as-a-convention-for-everyday-programming
    const uint32_t WIDTH = 800, HEIGHT = 600;
    GLFWwindow* window;
    VkInstance instance;
    
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif
    
    void initWindow() {
        glfwInit();
        
        // Config GLFW to not create a OpenGL context
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // Block resizing
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        
        window = glfwCreateWindow(WIDTH, HEIGHT, "3d engine", nullptr, nullptr);
    }
    
    void printAvailableExtensions() {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
        
        std::cout << "Available extensions:\n";
        for(const auto& extension : extensions){
            std::cout << '\t' << extension.extensionName << '\n';
        }
    }
    
    bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        
        for(const char* layerName : validationLayers) {
            bool layerFound = false;
            
            for(const auto& layerProperties : availableLayers) {
                if(strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }
            
            if(!layerFound) {
                return false;
            }
        }
        
        return true;
    }
    
    void createInstance(){
        if(enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("Validation layers requested but not available.");
        }
        
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "3d Engine";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
        
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        
        uint32_t glfwExtensionCount = 0;
        // Poiter to pointer "**" means
        const char** glfwExtensions;
        
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        
        if(enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        } else {
            createInfo.enabledLayerCount = 0;
        }
        
        // start Apple compatibility
        std::vector<const char*> requiredExtensions;

        for(uint32_t i = 0; i < glfwExtensionCount; i++) {
            requiredExtensions.emplace_back(glfwExtensions[i]);
        }

        requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

        createInfo.enabledExtensionCount = (uint32_t) requiredExtensions.size();
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();
        // end Apple compatibility
        
        printAvailableExtensions();
        
        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
        
        std::cout << "vkResult: " << result << std::endl;
        if(result != VK_SUCCESS){
            throw std::runtime_error("Failed to create instance!");
        }
    }
    
    void initVulkan() {
        createInstance();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
