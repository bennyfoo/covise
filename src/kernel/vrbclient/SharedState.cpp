/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

#include "SharedState.h"
#include <vrbclient/regClass.h>
#include "SharedStateManager.h"
#include <vrbclient/VrbClientRegistry.h>
#include <chrono>
#include <ctime>



namespace vrb
{

SharedStateBase::SharedStateBase(const std::string name, SharedStateType mode, const std::string& className)
    : m_registry(SharedStateManager::instance()->getRegistry())
    , variableName(name)
	, m_className(className)
{
    auto news = SharedStateManager::instance()->add(this, mode);
    sessionID = news.first;
    muted = news.second;
}

SharedStateBase::~SharedStateBase()
{
    if(m_registry)
    {
        m_registry->unsubscribeVar(m_className, variableName);
    }
    SharedStateManager::instance()->remove(this);
}

void SharedStateBase::subscribe(DataHandle & val)
{
    if(m_registry)
    {
        m_registry->subscribeVar(sessionID, m_className, variableName, val, this);
    }
}

void SharedStateBase::setVar(DataHandle & val)
{
    m_valueData = val;
	if (syncInterval <= 0)
	{
		m_registry->setVar(sessionID, m_className, variableName, m_valueData, muted);
	}
	else
	{
		send = true;
	}

}

void SharedStateBase::setUpdateFunction(std::function<void()> function)
{
    updateCallback = function;
}

bool SharedStateBase::valueChangedByOther() const
{
    return valueChanged;
}

std::string SharedStateBase::getName() const
{
    return variableName;
}

void SharedStateBase::update(clientRegVar *theChangedVar)
{
    if (theChangedVar->getName() != variableName || theChangedVar->isDeleted())
    {
        return;
    }
    covise::TokenBuffer tb(theChangedVar->getValue().data(), theChangedVar->getValue().length());
    deserializeValue(tb);
    valueChanged = true;
    if (updateCallback != nullptr)
    {
        updateCallback();
    }
}

void SharedStateBase::setID(SessionID &id)
{
    sessionID = id;
}

void SharedStateBase::setMute(bool m)
{
    muted = m;
}

bool SharedStateBase::getMute()
{
    return muted;
}

void SharedStateBase::resubscribe(SessionID &id)
{
    if (m_registry && m_registry->getClass(m_className)->getVar(variableName))
    {
        m_registry->unsubscribeVar(m_className, variableName, true);
    }

    m_registry->subscribeVar(id, m_className, variableName, m_valueData, this);
}

void SharedStateBase::frame(double time)
{
    if(m_registry == NULL)
        return;
    if (sessionID == SessionID())
    {
        return;
    }
    if (send && time >= lastUpdateTime + syncInterval)
    {
        m_registry->setVar(sessionID, m_className, variableName, m_valueData, muted);
        lastUpdateTime = time;
        send = false;
    }
}

void SharedStateBase::setSyncInterval(float time)
{
    syncInterval = time;
}

float SharedStateBase::getSyncInerval()
{
    return syncInterval;
}
void SharedStateBase::becomeMaster()
{
    muted = false;
    if (m_valueData.length() > 0)
    {
        send = true;
    }
}
}
