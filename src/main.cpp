//
//  main.cpp
//  candide
//
//  Created by Damian Stewart on 03.11.13.
//  Copyright (c) 2013 Damian Stewart. All rights reserved.
//

#include <iostream>
#include <unistd.h>

#include "CDApp.h"

int main(int argc, const char * argv[])
{
	
	char buf[4096];
	getcwd(buf, 4096);
	std::cout<<"cwd: "<<buf<<std::endl;
	
	try {
		
		CDApp candideApp(argc, argv);
		
		return candideApp.run();
		
	} catch ( const std::exception &e ) {
		
		std::cerr << "Unhandled exception: "<< e.what() << std::endl;
	}
}

