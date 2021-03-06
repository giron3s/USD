//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#ifndef PXRUSDMAYA_PLUGINSTATICDATA_H
#define PXRUSDMAYA_PLUGINSTATICDATA_H

#include "pxr/pxr.h"
#include "usdMaya/proxyShape.h"
#include "usdMaya/referenceAssembly.h"
#include "usdMaya/stageData.h"

#include <boost/noncopyable.hpp>

PXR_NAMESPACE_OPEN_SCOPE


/// \brief This class is solely here to coordinate between the various Usd
/// plugin node typeIds and typeNames.  The constructor makes sure that all the
/// appropriate class' StaticData objects get the correct data.
///
/// At some point, the shapes can move to the plugin, but we may want other
/// plugins to be able to identify and reason about things like UsdMayaStageData.
///
/// Normally the data would just be static members of the class.  However, since
/// we want to support our old maya files that still have old node type
/// ids/names, we move the data here so that we can use the same class for two
/// different sets of type ids/names.  If we don't need to support these, we can
/// move the data back as static class members.
///
/// This members of this struct get modified by each class's initialize()
/// function and then a reference is held by each class through the creator().
class PxrUsdMayaPluginStaticData : public boost::noncopyable {
public:
    PxrUsdMayaPluginStaticData(
            const MTypeId& proxyShapeId,
            const MString& proxyShapeName,

            const MTypeId& refAssemblyId,
            const MString& refAssemblyName,

            const MTypeId& stageDataId,
            const MString& stageDataName);

    UsdMayaProxyShape::PluginStaticData proxyShape;
    UsdMayaReferenceAssembly::PluginStaticData referenceAssembly;
    UsdMayaStageData::PluginStaticData stageData;

    // instance of PxrUsdMayaPluginStaticData used by the "pxrUsd" plugin.
    // Should not be used until the pxrUsd class is initialized.
    static PxrUsdMayaPluginStaticData pxrUsd;
};


PXR_NAMESPACE_CLOSE_SCOPE

#endif // PXRUSDMAYA_PLUGINSTATICDATA_H
