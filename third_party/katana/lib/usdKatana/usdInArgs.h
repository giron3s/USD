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
#ifndef PXRUSDKATANA_USDIN_ARGS_H
#define PXRUSDKATANA_USDIN_ARGS_H

#include "pxr/pxr.h"
#include "pxr/usd/usdGeom/bboxCache.h"
#include "pxr/base/tf/refPtr.h"

#include <tbb/enumerable_thread_specific.h>

/// \brief Reference counted container for op state that should be constructed
/// at an ops root and passed to read USD prims into Katana attributes.
///
/// This should hold ref pointers or shareable copies of state that should not
/// be copied at each location.
///

#include <FnAttribute/FnAttribute.h>

PXR_NAMESPACE_OPEN_SCOPE

class PxrUsdKatanaUsdInArgs;
typedef TfRefPtr<PxrUsdKatanaUsdInArgs> PxrUsdKatanaUsdInArgsRefPtr;

/// The sessionAttr is a structured GroupAttribute argument for delivering
/// edits to the session layer of the stage. It replaces the earlier
/// "variants" string while also providing activate/deactivate edits.
/// 
/// It is typically delivered to the node via graphState -- which then provides
/// it directly here via opArg.
/// 
/// The format of the attr is:
/// |
/// ---variants
/// |  |
/// |  --- <entryPath>
/// |  |  |
/// |  | ---variantSetName -> variantSelection
/// |  |  |
/// |  |---...
/// |  ---...
/// ---activations
///    |
///    ---<entryPath> -> IntAttribute(0|1)
///    |
///    ---...
/// 
/// The format of <entryPath> is a FnAttribute::DelimiterEncode'd scenegraph
/// location path. This protects against invalid characters in the entry attr
/// name (as "." is valid in a scenegraph location path). The location path
/// is the full katana scene path (so that multiple instances of the op) can
/// distinguish what's relevant based on being beneath its own rootLocation
/// argument. Internally, a sessionLocation argument may be used to specify
/// a root separate from its own. This is typically use for cases of recursive
/// op expansion sharing the same cached stage.


class PxrUsdKatanaUsdInArgs : public TfRefBase
{

public:

    typedef std::map<std::string, std::vector<std::string> > StringListMap;

    static PxrUsdKatanaUsdInArgsRefPtr New(
            UsdStageRefPtr stage,
            const std::string& rootLocation,
            const std::string& isolatePath,
            FnAttribute::GroupAttribute sessionAttr,
            const std::string& ignoreLayerRegex,
            double currentTime,
            double shutterOpen,
            double shutterClose,
            const std::vector<double>& motionSampleTimes,
            const std::set<std::string>& defaultMotionPaths,
            const StringListMap& extraAttributesOrNamespaces,
            bool verbose,
            const char * errorMessage = 0) {
        return TfCreateRefPtr(new PxrUsdKatanaUsdInArgs(
                    stage, 
                    rootLocation,
                    isolatePath,
                    sessionAttr,
                    ignoreLayerRegex,
                    currentTime,
                    shutterOpen,
                    shutterClose, 
                    motionSampleTimes,
                    defaultMotionPaths,
                    extraAttributesOrNamespaces,
                    verbose,
                    errorMessage));
    }

    // bounds computation is kind of important, so we centralize it here.
    std::vector<GfBBox3d> ComputeBounds(const UsdPrim& prim);

    UsdPrim GetRootPrim() const;

    UsdStageRefPtr GetStage() const {
        return _stage;
    }

    std::string GetFileName() const {
        return _stage->GetRootLayer()->GetIdentifier();
    }

    const std::string& GetRootLocationPath() const {
        return _rootLocation;
    }

    const std::string& GetIsolatePath() const {
        return _isolatePath;
    }

    FnAttribute::GroupAttribute GetSessionAttr() {
        return _sessionAttr;
    }

    const std::string& GetIgnoreLayerRegex() const {
        return _ignoreLayerRegex;
    }

    double GetCurrentTime() const {
        return _currentTime;
    }

    double GetShutterOpen() const {
        return _shutterOpen;
    }

    double GetShutterClose() const {
        return _shutterClose;
    }

    const std::vector<double>& GetMotionSampleTimes() const {
        return _motionSampleTimes;
    }

    const std::set<std::string>& GetDefaultMotionPaths() const {
        return _defaultMotionPaths;
    }

    /// \brief Return true if motion blur is backward.
    ///
    /// PxrUsdIn supports both forward and backward motion blur. Motion
    /// blur is considered backward if multiple samples are requested
    /// and the first specified sample is later than the last sample.
    const bool IsMotionBackward() const {
        return _isMotionBackward;
    }

    const StringListMap& GetExtraAttributesOrNamespaces() const {
        return _extraAttributesOrNamespaces;
    }

    bool IsVerbose() const {
        return _verbose;
    }

    std::vector<UsdGeomBBoxCache>& GetBBoxCache() {
        return _bboxCaches.local();
    }

    const std::string & GetErrorMessage() {
        return _errorMessage;
    }
private:

    PxrUsdKatanaUsdInArgs(
            UsdStageRefPtr stage,
            const std::string& rootLocation,
            const std::string& isolatePath,
            FnAttribute::GroupAttribute sessionAttr,
            const std::string& ignoreLayerRegex,
            double currentTime,
            double shutterOpen,
            double shutterClose,
            const std::vector<double>& motionSampleTimes,
            const std::set<std::string>& defaultMotionPaths,
            const StringListMap& extraAttributesOrNamespaces,
            bool verbose,
            const char * errorMessage = 0);

    ~PxrUsdKatanaUsdInArgs();

    UsdStageRefPtr _stage;

    std::string _rootLocation;
    std::string _isolatePath;

    FnAttribute::GroupAttribute _sessionAttr;
    std::string _ignoreLayerRegex;

    double _currentTime;
    double _shutterOpen;
    double _shutterClose;
    std::vector<double> _motionSampleTimes;
    std::set<std::string> _defaultMotionPaths;
    bool _isMotionBackward;

    // maps the root-level attribute name to the specified attributes or namespaces
    StringListMap _extraAttributesOrNamespaces;

    bool _verbose;

    typedef tbb::enumerable_thread_specific< std::vector<UsdGeomBBoxCache> > _ThreadLocalBBoxCaches;
    _ThreadLocalBBoxCaches _bboxCaches;
    
    std::string _errorMessage;

};


// utility to make it easier to exit earlier from InitUsdInArgs
struct ArgsBuilder
{
    UsdStageRefPtr stage;
    std::string rootLocation;
    std::string isolatePath;
    FnAttribute::GroupAttribute sessionAttr;
    std::string ignoreLayerRegex;
    double currentTime;
    double shutterOpen;
    double shutterClose;
    std::vector<double> motionSampleTimes;
    std::set<std::string> defaultMotionPaths;
    PxrUsdKatanaUsdInArgs::StringListMap extraAttributesOrNamespaces;
    bool verbose;
    const char * errorMessage;
    
    
    ArgsBuilder()
    : currentTime(0.0)
    , shutterOpen(0.0)
    , shutterClose(0.0)
    , verbose(false)
    , errorMessage(0)
    {
    }
    
    PxrUsdKatanaUsdInArgsRefPtr build()
    {
        return PxrUsdKatanaUsdInArgs::New(
            stage,
            rootLocation,
            isolatePath,
            sessionAttr.isValid() ? sessionAttr :
                    FnAttribute::GroupAttribute(true),
            ignoreLayerRegex,
            currentTime,
            shutterOpen,
            shutterClose,
            motionSampleTimes,
            defaultMotionPaths,
            extraAttributesOrNamespaces,
            verbose,
            errorMessage);
    }
    
    PxrUsdKatanaUsdInArgsRefPtr buildWithError(std::string errorStr)
    {
        errorMessage = errorStr.c_str();
        return build();
    }
    
};


PXR_NAMESPACE_CLOSE_SCOPE

#endif // PXRUSDKATANA_USDIN_ARGS_H
