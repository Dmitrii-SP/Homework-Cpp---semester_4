#include <iostream>
#include <dlfcn.h> 
#include <unistd.h> 

int main() {
    std::cout << "Программа запущена. Ждем 2 секунды..." << std::endl;
    sleep(2); 

    void* handle = dlopen("./libmiptlib.so", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Ошибка загрузки библиотеки: " << dlerror() << std::endl;
        return 1;
    }

    std::cout << "Библиотека успешно загружена!" << std::endl;
    
    dlclose(handle);
    std::cout << "Библиотека выгружена. Завершение работы." << std::endl;

    return 0;
}