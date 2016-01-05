//
// Created by Jude Pereira on 05/01/2016.
//

#pragma once

#include <iostream>
#include "ofConstants.h"
#include "ofLog.h"
#include "Utils.h"

using namespace std;

class CMakeGenerator {
private:
    string projectPath;
    ofTargetPlatform platform;
    vector<string> addons;

public:
    CMakeGenerator(ofTargetPlatform, string, vector<string> &);

    void generate();
};