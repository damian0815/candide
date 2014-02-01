//
//  CDUtilities.h
//  candide
//
//  Created by damian on 03/11/13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#ifndef __candide__CDUtilities__
#define __candide__CDUtilities__

#include <iostream>
#include <sstream>
#include <glm/glm.hpp>
#include "picojson.h"

#define CDLog CDLogInternal() << __PRETTY_FUNCTION__ << ": "
//#define CDLogErr CDLogInternal << __PRETTY_FUNCTION__ << ": error: "


class CDLogInternal {
public:
	CDLogInternal() {}
    ~CDLogInternal() {
        std::cout << logText.str() << std::endl;
    }
	
    // accepts just about anything
    template<class T>
    CDLogInternal &operator<<(const T &x) {
        logText << x;
        return *this;
    }
private:
    std::ostringstream logText;
};


picojson::value picojson_encodeVector3( const glm::vec3& vec );
glm::vec3 picojson_decodeVector3( const picojson::value& source );



#endif /* defined(__candide__CDUtilities__) */
