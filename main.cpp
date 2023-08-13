#include <memory>
#include <iostream>

#include "Application.h"

using namespace rgaa;

int main() {

    auto app = std::make_shared<Application>();
    app->Init();
    app->Start();

    return 0;
}
