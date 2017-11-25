/* This file is part of COVISE.

You can use it under the terms of the GNU Lesser General Public License
version 2.1 or later, see lgpl - 2.1.txt.

* License: LGPL 2 + */


#ifndef OSCSTART_H
#define OSCSTART_H

#include "../oscExport.h"
#include "../oscObjectBase.h"
#include "../oscObjectVariable.h"
#include "../oscObjectVariableArray.h"

#include "../oscVariables.h"
#include "oscStartConditionGroup.h"

namespace OpenScenario
{
class OPENSCENARIOEXPORT oscStart : public oscObjectBase
{
public:
oscStart()
{
        OSC_OBJECT_ADD_MEMBER(ConditionGroup, "oscStartConditionGroup", 0);
    };
        const char *getScope(){return "/OSCManeuver/Event/Conditions";};
    oscStartConditionGroupMember ConditionGroup;

};

typedef oscObjectVariable<oscStart *> oscStartMember;
typedef oscObjectVariableArray<oscStart *> oscStartArrayMember;


}

#endif //OSCSTART_H
