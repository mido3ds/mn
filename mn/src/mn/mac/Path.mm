#import <Foundation/Foundation.h>

const char*
get_home_path()
{
	return [NSHomeDirectory() UTF8String];
}

const char* get_tmp_path()
{
	return [NSTemporaryDirectory() UTF8String];
}