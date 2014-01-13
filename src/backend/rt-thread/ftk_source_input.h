/*
 * ftk_source_touch.h
 *
 *  Created on: 2011-5-23
 *      Author: Yi.Qiu
 */

#ifndef FTK_SOURCE_INPUT_H_
#define FTK_SOURCE_INPUT_H_

#include "ftk_event.h"
#include "ftk_source.h"

#define FTK_INPUT_NAME "key"

FtkSource* ftk_source_input_create(const char* filename, FtkOnEvent on_event, void* user_data);

#endif /* FTK_SOURCE_INPUT_H_ */
