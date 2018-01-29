/** ********************************************************************
 * @file   Lock.h
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
#ifndef FREERTOS_FREERTOSPP_LOCK_H_
#define FREERTOS_FREERTOSPP_LOCK_H_

#include "FreeRTOS.h"

/*
 *
 */

class Lockable {
public:
	Lockable() {}
	virtual ~Lockable() {}

	virtual bool take(TickType_t wait = portMAX_DELAY) = 0;
	virtual bool give() = 0;
private:
#if __cplusplus < 201101L
    Lockable(Lockable const&);      ///< We are not copyable.
    void operator =(Lockable const&);  ///< We are not assignable.
#else
    Lockable(Lockable const&) = delete;      ///< We are not copyable.
    void operator =(Lockable const&) = delete;  ///< We are not assignable.
#endif // __cplusplus
};

class Lock {
public:
	Lock(Lockable& mylockable, bool locked = true, TickType_t wait = portMAX_DELAY);
	virtual ~Lock();

	bool lock(TickType_t wait = portMAX_DELAY);
	void unlock();
	bool locked() const { return lockCnt > 0; }
private:
	Lockable& lockable;
	int		lockCnt;

#if __cplusplus < 201101L
    Lock(Lock const&);      ///< We are not copyable.
    void operator =(Lock const&);  ///< We are not assignable.
#else
    Lock(Lock const&) = delete;      ///< We are not copyable.
    void operator =(Lock const&) = delete;  ///< We are not assignable.
#endif // __cplusplus
};

#endif /* FREERTOS_FREERTOSPP_LOCK_H_ */
