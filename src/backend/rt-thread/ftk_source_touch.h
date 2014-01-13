/*
 * ftk_source_touch.h
 *
 *  Created on: 2010-9-15
 *      Author: jiaojinxing
 */

#ifndef FTK_SOURCE_TOUCH_H_
#define FTK_SOURCE_TOUCH_H_

#include "ftk_event.h"
#include "ftk_source.h"

#define FTK_TOUCH_NAME "touch"

FtkSource* ftk_source_touch_create(const char* filename, FtkOnEvent on_event, void* user_data);

#endif /* FTK_SOURCE_TOUCH_H_ */
