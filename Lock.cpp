/** ********************************************************************
 * @file   Lock.cpp
 * @author rdamon
 * @copyright Copyright (c) 2017. Optics 1, Inc. All Rights Reserved.
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose, without fee and without a signed
 * licensing agreement, is hereby prohibited. Any unauthorized
 * reproduction or distribution of this software, or any portion of it,
 * may result in severe civil and criminal penalties, and will be
 * prosecuted to the maximum extent possible under the law.
 * \n\n
 * This document may contain technical data whose export is restricted by
 * the U.S. Department of State, under the Arms Export Control Act
 * (Title 22, U.S.C., Section 2751) and the International Traffic in Arms
 * Regulations (Title 22 CFR §120-130). Licenses approved by the Department of
 * State are required for the export (as defined in ITAR 22 CFR §120.17) of
 * such technical data to any foreign person or organization (as defined in
 * ITAR 22 CFR §120.16) either in the United States or abroad. Further
 * distribution of any technical data contained in this document, without the
 * prior written approval of the Department of State, is a violation of these
 * export laws and could subject the offender to severe criminal penalties.
 * Reference TA-7603-10A
 * \n\n
 * Use, duplication, or disclosure of this sheet is subject to the restrictions
 * above.
 * \n\n
 * Paper copies of this document are not controlled and are for reference only.
 * All references to this document shall be taken from the electronic &
 * controlled format
 * OPTICS1 Proprietary Information
 *
 * @date Jun 6, 2017 Created
 *
 *  Description:
 * @brief
 *
 **********************************************************************/
#include <Lock.h>
#include "IOLib/log.h"

Lock::Lock(Lockable& myLockable, bool mylocked, TickType_t wait) :
lockable(myLockable),
lockCnt(0)
{
	if(mylocked) lock(wait);
}

Lock::~Lock() {
	// on destruct, remove all locks.
	if(lockCnt > 0) {
		lockable.give();
	}
	lockCnt = 0;
}

bool Lock::lock(TickType_t wait) {
	if(lockCnt > 0) {
		lockCnt++;
		return true;
	}
	if(lockable.take(wait)) {
		lockCnt++;
		return true;
	}
	return false;
}

void Lock::unlock() {
	if(lockCnt > 0) {	// ignore extra unlocks.
		lockCnt--;
		if(lockCnt == 0) {
			lockable.give();
		}
	}
}
