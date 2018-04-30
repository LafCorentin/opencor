/*******************************************************************************

Copyright (C) The University of Auckland

OpenCOR is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenCOR is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/

//==============================================================================
// Simulation support plugin
//==============================================================================

#pragma once

//==============================================================================

#include "i18ninterface.h"
#include "plugininfo.h"
#include "simulationinterface.h"

//==============================================================================

namespace OpenCOR {
namespace SimulationSupport {

//==============================================================================

PLUGININFO_FUNC SimulationSupportPluginInfo();

//==============================================================================

class SimulationSupportPlugin : public QObject, public I18nInterface,
                                public SimulationInterface
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "OpenCOR.SimulationSupportPlugin" FILE "simulationsupportplugin.json")

    Q_INTERFACES(OpenCOR::I18nInterface)
    Q_INTERFACES(OpenCOR::SimulationInterface)

public:
#include "i18ninterface.inl"
#include "simulationinterface.inl"
};

//==============================================================================

}   // namespace SimulationSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
