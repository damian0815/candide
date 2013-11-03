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


#endif /* defined(__candide__CDUtilities__) */
