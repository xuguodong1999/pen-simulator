# skia
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/skia-src/skia)
set(INC_DIR ${ROOT_DIR}/include)
set(SRC_DIR ${ROOT_DIR}/src)

#############################
# This file contains lists of files and defines used in the legacy G3 build that is the G3 build
# that is not derived from our Bazel rules.

# All platform-independent sources and private headers.
set(ENCODE_SRCS
        src/encode/SkEncoder.cpp
        src/encode/SkICCPriv.h
        src/encode/SkICC.cpp
        src/encode/SkImageEncoderFns.h
        src/encode/SkImageEncoderPriv.h)

set(ENCODE_JPEG_SRCS
        src/encode/SkJpegEncoderImpl.h
        src/encode/SkJpegEncoderImpl.cpp
        src/encode/SkJPEGWriteUtility.cpp
        src/encode/SkJPEGWriteUtility.h)

set(NO_ENCODE_JPEG_SRCS
        src/encode/SkJpegEncoder_none.cpp)

set(ENCODE_PNG_SRCS
        src/encode/SkPngEncoderImpl.cpp
        src/encode/SkPngEncoderImpl.h)

set(NO_ENCODE_PNG_SRCS
        src/encode/SkPngEncoder_none.cpp)

set(ENCODE_WEBP_SRCS
        src/encode/SkWebpEncoderImpl.cpp)

set(NO_ENCODE_WEBP_SRCS
        src/encode/SkWebpEncoder_none.cpp)

set(CODEC_SRCS_LIMITED
        src/codec/SkAndroidCodec.cpp
        src/codec/SkAndroidCodecAdapter.cpp
        src/codec/SkAndroidCodecAdapter.h
        src/codec/SkBmpBaseCodec.cpp
        src/codec/SkBmpBaseCodec.h
        src/codec/SkBmpCodec.cpp
        src/codec/SkBmpCodec.h
        src/codec/SkBmpMaskCodec.cpp
        src/codec/SkBmpMaskCodec.h
        src/codec/SkBmpRLECodec.cpp
        src/codec/SkBmpRLECodec.h
        src/codec/SkBmpStandardCodec.cpp
        src/codec/SkBmpStandardCodec.h
        src/codec/SkCodec.cpp
        src/codec/SkCodecImageGenerator.cpp
        src/codec/SkCodecImageGenerator.h
        src/codec/SkCodecPriv.h
        src/codec/SkColorPalette.cpp
        src/codec/SkColorPalette.h
        src/codec/SkExif.cpp
        src/codec/SkEncodedInfo.cpp
        src/codec/SkFrameHolder.h
        src/codec/SkImageGenerator_FromEncoded.cpp
        src/codec/SkJpegCodec.cpp
        src/codec/SkJpegCodec.h
        src/codec/SkJpegDecoderMgr.cpp
        src/codec/SkJpegDecoderMgr.h
        src/codec/SkJpegPriv.h
        src/codec/SkJpegSourceMgr.cpp
        src/codec/SkJpegSourceMgr.h
        src/codec/SkJpegUtility.cpp
        src/codec/SkJpegUtility.h
        src/codec/SkMasks.cpp
        src/codec/SkMasks.h
        src/codec/SkMaskSwizzler.cpp
        src/codec/SkMaskSwizzler.h
        src/codec/SkParseEncodedOrigin.cpp
        src/codec/SkParseEncodedOrigin.h
        src/codec/SkPngPriv.h
        src/codec/SkSampledCodec.cpp
        src/codec/SkSampledCodec.h
        src/codec/SkScalingCodec.h
        src/codec/SkSampler.cpp
        src/codec/SkSampler.h
        src/codec/SkSwizzler.cpp
        src/codec/SkSwizzler.h
        src/codec/SkTiffUtility.cpp
        src/codec/SkWbmpCodec.cpp
        src/codec/SkWbmpCodec.h
        src/codec/SkWuffsCodec.cpp
        src/codec/SkWuffsCodec.h)

set(CODEC_SRCS_ALL ${CODEC_SRCS_LIMITED}
        src/codec/SkIcoCodec.cpp
        src/codec/SkIcoCodec.h
        src/codec/SkPngCodec.cpp
        src/codec/SkPngCodec.h
        src/codec/SkWebpCodec.cpp
        src/codec/SkWebpCodec.h)

set(TEXTUAL_HDRS
        src/sksl/generated/sksl_compute.minified.sksl
        src/sksl/generated/sksl_compute.unoptimized.sksl
        src/sksl/generated/sksl_frag.minified.sksl
        src/sksl/generated/sksl_frag.unoptimized.sksl
        src/sksl/generated/sksl_gpu.minified.sksl
        src/sksl/generated/sksl_gpu.unoptimized.sksl
        src/sksl/generated/sksl_graphite_frag.minified.sksl
        src/sksl/generated/sksl_graphite_frag.unoptimized.sksl
        src/sksl/generated/sksl_graphite_vert.minified.sksl
        src/sksl/generated/sksl_graphite_vert.unoptimized.sksl
        src/sksl/generated/sksl_public.minified.sksl
        src/sksl/generated/sksl_public.unoptimized.sksl
        src/sksl/generated/sksl_rt_shader.minified.sksl
        src/sksl/generated/sksl_rt_shader.unoptimized.sksl
        src/sksl/generated/sksl_shared.minified.sksl
        src/sksl/generated/sksl_shared.unoptimized.sksl
        src/sksl/generated/sksl_vert.minified.sksl
        src/sksl/generated/sksl_vert.unoptimized.sksl
        # Included by GrGLMakeNativeInterface_android.cpp
        src/gpu/ganesh/gl/egl/GrGLMakeEGLInterface.cpp
        src/gpu/ganesh/gl/egl/GrGLMakeNativeInterface_egl.cpp)

set(base_gl_srcs
        src/gpu/ganesh/gl/GrGLAssembleGLESInterfaceAutogen.cpp
        src/gpu/ganesh/gl/GrGLAssembleGLInterfaceAutogen.cpp
        src/gpu/ganesh/gl/GrGLAssembleHelpers.cpp
        src/gpu/ganesh/gl/GrGLAssembleInterface.cpp
        src/gpu/ganesh/gl/GrGLAssembleWebGLInterfaceAutogen.cpp
        src/gpu/ganesh/gl/GrGLAttachment.cpp
        src/gpu/ganesh/gl/GrGLAttachment.h
        src/gpu/ganesh/gl/GrGLBackendSurface.cpp
        src/gpu/ganesh/gl/GrGLBackendSurfacePriv.h
        src/gpu/ganesh/gl/GrGLBuffer.cpp
        src/gpu/ganesh/gl/GrGLBuffer.h
        src/gpu/ganesh/gl/GrGLCaps.cpp
        src/gpu/ganesh/gl/GrGLCaps.h
        src/gpu/ganesh/gl/GrGLContext.cpp
        src/gpu/ganesh/gl/GrGLContext.h
        src/gpu/ganesh/gl/GrGLDefines.h
        src/gpu/ganesh/gl/GrGLDirectContext.cpp
        src/gpu/ganesh/gl/GrGLExtensions.cpp
        src/gpu/ganesh/gl/GrGLGLSL.cpp
        src/gpu/ganesh/gl/GrGLGLSL.h
        src/gpu/ganesh/gl/GrGLGpu.cpp
        src/gpu/ganesh/gl/GrGLGpu.h
        src/gpu/ganesh/gl/GrGLGpuProgramCache.cpp
        src/gpu/ganesh/gl/GrGLInterfaceAutogen.cpp
        src/gpu/ganesh/gl/GrGLOpsRenderPass.cpp
        src/gpu/ganesh/gl/GrGLOpsRenderPass.h
        src/gpu/ganesh/gl/GrGLProgram.cpp
        src/gpu/ganesh/gl/GrGLProgram.h
        src/gpu/ganesh/gl/GrGLProgramDataManager.cpp
        src/gpu/ganesh/gl/GrGLProgramDataManager.h
        src/gpu/ganesh/gl/GrGLRenderTarget.cpp
        src/gpu/ganesh/gl/GrGLRenderTarget.h
        src/gpu/ganesh/gl/GrGLSemaphore.cpp
        src/gpu/ganesh/gl/GrGLSemaphore.h
        src/gpu/ganesh/gl/GrGLTexture.cpp
        src/gpu/ganesh/gl/GrGLTexture.h
        src/gpu/ganesh/gl/GrGLTextureRenderTarget.cpp
        src/gpu/ganesh/gl/GrGLTextureRenderTarget.h
        src/gpu/ganesh/gl/GrGLTypesPriv.cpp
        src/gpu/ganesh/gl/GrGLTypesPriv.h
        src/gpu/ganesh/gl/GrGLUniformHandler.cpp
        src/gpu/ganesh/gl/GrGLUniformHandler.h
        src/gpu/ganesh/gl/GrGLUtil.cpp
        src/gpu/ganesh/gl/GrGLUtil.h
        src/gpu/ganesh/gl/GrGLVaryingHandler.h
        src/gpu/ganesh/gl/GrGLVertexArray.cpp
        src/gpu/ganesh/gl/GrGLVertexArray.h
        src/gpu/ganesh/gl/builders/GrGLProgramBuilder.cpp
        src/gpu/ganesh/gl/builders/GrGLProgramBuilder.h
        src/gpu/ganesh/gl/builders/GrGLShaderStringBuilder.cpp
        src/gpu/ganesh/gl/builders/GrGLShaderStringBuilder.h)

set(GL_SRCS_UNIX ${base_gl_srcs}
        src/gpu/ganesh/gl/GrGLMakeNativeInterface_none.cpp)

set(GL_SRCS_UNIX_EGL ${base_gl_srcs}
        src/gpu/ganesh/gl/egl/GrGLMakeEGLInterface.cpp
        src/gpu/ganesh/gl/egl/GrGLMakeNativeInterface_egl.cpp)

set(PORTS_SRCS_WIN
        src/ports/SkFontMgr_win_dw_factory.cpp
        src/fonts/SkFontMgr_indirect.cpp
        src/ports/SkFontMgr_win_dw.cpp
        src/ports/SkScalerContext_win_dw.cpp
        src/ports/SkTypeface_win_dw.cpp
        src/ports/SkGlobalInitialization_default.cpp
        src/ports/SkOSFile_win.cpp
        src/ports/SkOSFile_stdio.cpp
        src/ports/SkDebug_win.cpp
        src/ports/SkMemory_malloc.cpp)
set(PORTS_SRCS_UNIX
        src/ports/SkDebug_stdio.cpp
        src/ports/SkFontHost_FreeType_common.cpp
        src/ports/SkFontHost_FreeType_common.h
        src/ports/SkFontHost_FreeType.cpp
        src/ports/SkFontMgr_custom.cpp
        src/ports/SkFontMgr_custom_directory.cpp
        src/ports/SkFontMgr_custom_embedded.cpp
        src/ports/SkFontMgr_custom_empty.cpp
        src/ports/SkFontMgr_custom.h
        src/ports/SkGlobalInitialization_default.cpp
        src/ports/SkMemory_malloc.cpp
        src/ports/SkOSFile_posix.cpp
        src/ports/SkOSFile_stdio.cpp
        src/ports/SkOSLibrary.h
        #        src/ports/SkOSLibrary_posix.cpp
)
if (XGD_USE_FONTCONFIG)
    list(APPEND PORTS_SRCS_UNIX
            src/ports/SkFontMgr_fontconfig_factory.cpp
            src/ports/SkFontMgr_fontconfig.cpp)
else ()
    list(APPEND PORTS_SRCS_UNIX
            src/ports/SkFontMgr_empty_factory.cpp)
endif ()

set(GL_SRCS_ANDROID ${base_gl_srcs}
        src/gpu/ganesh/gl/android/GrGLMakeNativeInterface_android.cpp)

set(PORTS_SRCS_ANDROID
        src/ports/SkDebug_android.cpp
        src/ports/SkFontHost_FreeType_common.cpp
        src/ports/SkFontHost_FreeType_common.h
        src/ports/SkFontHost_FreeType.cpp
        src/ports/SkFontMgr_android.cpp
        src/ports/SkFontMgr_android_factory.cpp
        src/ports/SkFontMgr_android_parser.cpp
        src/ports/SkFontMgr_android_parser.h
        src/ports/SkFontMgr_custom.cpp
        src/ports/SkFontMgr_custom_directory.cpp
        src/ports/SkFontMgr_custom_embedded.cpp
        src/ports/SkFontMgr_custom_empty.cpp
        src/ports/SkFontMgr_custom.h
        src/ports/SkGlobalInitialization_default.cpp
        src/ports/SkMemory_malloc.cpp
        src/ports/SkOSFile_posix.cpp
        src/ports/SkOSFile_stdio.cpp
        src/ports/SkOSLibrary.h
        src/ports/SkOSLibrary_posix.cpp)

set(PORTS_SRCS_ANDROID_NO_FONT
        src/ports/SkDebug_android.cpp
        src/ports/SkFontMgr_empty_factory.cpp
        src/ports/SkGlobalInitialization_default.cpp
        src/ports/SkMemory_malloc.cpp
        src/ports/SkOSFile_posix.cpp
        src/ports/SkOSFile_stdio.cpp
        src/ports/SkOSLibrary.h
        src/ports/SkOSLibrary_posix.cpp)

set(GL_SRCS_IOS ${base_gl_srcs}
        src/gpu/ganesh/gl/iOS/GrGLMakeNativeInterface_iOS.cpp)

set(PORTS_SRCS_IOS
        src/ports/SkDebug_stdio.cpp
        src/ports/SkFontMgr_custom.h
        src/ports/SkFontMgr_mac_ct.cpp
        src/ports/SkFontMgr_mac_ct_factory.cpp
        src/ports/SkGlobalInitialization_default.cpp
        src/ports/SkImageGeneratorCG.cpp
        src/ports/SkMemory_malloc.cpp
        src/ports/SkOSFile_ios.h
        src/ports/SkOSFile_posix.cpp
        src/ports/SkOSFile_stdio.cpp
        src/ports/SkOSLibrary.h
        src/ports/SkOSLibrary_posix.cpp
        src/ports/SkScalerContext_mac_ct.cpp
        src/ports/SkScalerContext_mac_ct.h
        src/ports/SkTypeface_mac_ct.cpp
        src/ports/SkTypeface_mac_ct.h
        src/utils/mac/SkCGBase.h
        src/utils/mac/SkCGGeometry.h
        src/utils/mac/SkCreateCGImageRef.cpp
        src/utils/mac/SkCTFont.cpp
        src/utils/mac/SkCTFont.h
        src/utils/mac/SkUniqueCFRef.h)

set(PORTS_SRCS_FUCHSIA
        src/ports/SkDebug_stdio.cpp
        src/ports/SkFontHost_FreeType_common.cpp
        src/ports/SkFontHost_FreeType_common.h
        src/ports/SkFontHost_FreeType.cpp
        src/ports/SkFontMgr_custom.cpp
        src/ports/SkFontMgr_custom.h
        src/ports/SkFontMgr_empty_factory.cpp
        src/ports/SkFontMgr_fuchsia.cpp
        src/ports/SkGlobalInitialization_default.cpp
        src/ports/SkMemory_malloc.cpp
        src/ports/SkOSFile_posix.cpp
        src/ports/SkOSFile_stdio.cpp
        src/ports/SkOSLibrary.h
        src/ports/SkOSLibrary_posix.cpp)

set(GL_SRCS_MACOS ${base_gl_srcs}
        src/gpu/ganesh/gl/mac/GrGLMakeNativeInterface_mac.cpp)

set(PORTS_SRCS_MACOS ${PORTS_SRCS_IOS})

set(PORTS_SRCS_WASM
        src/ports/SkDebug_stdio.cpp
        src/ports/SkFontHost_FreeType_common.cpp
        src/ports/SkFontHost_FreeType_common.h
        src/ports/SkFontHost_FreeType.cpp
        src/ports/SkFontMgr_custom.cpp
        src/ports/SkFontMgr_custom.h
        src/ports/SkFontMgr_custom_embedded.cpp
        src/ports/SkFontMgr_empty_factory.cpp
        src/ports/SkGlobalInitialization_default.cpp
        src/ports/SkMemory_malloc.cpp
        src/ports/SkOSFile_posix.cpp
        src/ports/SkOSFile_stdio.cpp
        src/ports/SkOSLibrary.h
        src/ports/SkOSLibrary_posix.cpp)
set(GL_SRCS_WASM ${GL_SRCS_UNIX_EGL})

set(MTL_HDRS
        src/gpu/ganesh/mtl/GrMtlAttachment.h
        src/gpu/ganesh/mtl/GrMtlBuffer.h
        src/gpu/ganesh/mtl/GrMtlCaps.h
        src/gpu/ganesh/mtl/GrMtlCommandBuffer.h
        src/gpu/ganesh/mtl/GrMtlCppUtil.h
        src/gpu/ganesh/mtl/GrMtlDepthStencil.h
        src/gpu/ganesh/mtl/GrMtlFramebuffer.h
        src/gpu/ganesh/mtl/GrMtlGpu.h
        src/gpu/ganesh/mtl/GrMtlOpsRenderPass.h
        src/gpu/ganesh/mtl/GrMtlPipeline.h
        src/gpu/ganesh/mtl/GrMtlPipelineState.h
        src/gpu/ganesh/mtl/GrMtlPipelineStateBuilder.h
        src/gpu/ganesh/mtl/GrMtlPipelineStateDataManager.h
        src/gpu/ganesh/mtl/GrMtlRenderCommandEncoder.h
        src/gpu/ganesh/mtl/GrMtlRenderTarget.h
        src/gpu/ganesh/mtl/GrMtlResourceProvider.h
        src/gpu/ganesh/mtl/GrMtlSampler.h
        src/gpu/ganesh/mtl/GrMtlSemaphore.h
        src/gpu/ganesh/mtl/GrMtlTexture.h
        src/gpu/ganesh/mtl/GrMtlTextureRenderTarget.h
        src/gpu/ganesh/mtl/GrMtlTrampoline.h
        src/gpu/ganesh/mtl/GrMtlUniformHandler.h
        src/gpu/ganesh/mtl/GrMtlUtil.h
        src/gpu/ganesh/mtl/GrMtlVaryingHandler.h
        src/gpu/mtl/MtlMemoryAllocatorImpl.h)

set(MTL_SRCS
        src/gpu/ganesh/mtl/GrMtlAttachment.mm
        src/gpu/ganesh/mtl/GrMtlBuffer.mm
        src/gpu/ganesh/mtl/GrMtlCaps.mm
        src/gpu/ganesh/mtl/GrMtlCommandBuffer.mm
        src/gpu/ganesh/mtl/GrMtlDepthStencil.mm
        src/gpu/ganesh/mtl/GrMtlFramebuffer.mm
        src/gpu/ganesh/mtl/GrMtlGpu.mm
        src/gpu/ganesh/mtl/GrMtlOpsRenderPass.mm
        src/gpu/ganesh/mtl/GrMtlPipelineState.mm
        src/gpu/ganesh/mtl/GrMtlPipelineStateBuilder.mm
        src/gpu/ganesh/mtl/GrMtlPipelineStateDataManager.mm
        src/gpu/ganesh/mtl/GrMtlRenderTarget.mm
        src/gpu/ganesh/mtl/GrMtlResourceProvider.mm
        src/gpu/ganesh/mtl/GrMtlSampler.mm
        src/gpu/ganesh/mtl/GrMtlSemaphore.mm
        src/gpu/ganesh/mtl/GrMtlTexture.mm
        src/gpu/ganesh/mtl/GrMtlTextureRenderTarget.mm
        src/gpu/ganesh/mtl/GrMtlTrampoline.mm
        src/gpu/ganesh/mtl/GrMtlTypesPriv.h
        src/gpu/ganesh/mtl/GrMtlTypesPriv.mm
        src/gpu/ganesh/mtl/GrMtlUniformHandler.mm
        src/gpu/ganesh/mtl/GrMtlUtil.mm
        src/gpu/ganesh/mtl/GrMtlVaryingHandler.mm
        src/gpu/ganesh/surface/SkSurface_GaneshMtl.mm
        src/gpu/mtl/MtlMemoryAllocatorImpl.mm
        src/gpu/mtl/MtlUtils.mm
        src/gpu/mtl/MtlUtilsPriv.h)

set(VULKAN_SRCS
        src/gpu/ganesh/vk/GrVkBackendSurface.cpp
        src/gpu/ganesh/vk/GrVkBackendSurfacePriv.h
        src/gpu/ganesh/vk/GrVkBuffer.cpp
        src/gpu/ganesh/vk/GrVkBuffer.h
        src/gpu/ganesh/vk/GrVkCaps.cpp
        src/gpu/ganesh/vk/GrVkCaps.h
        src/gpu/ganesh/vk/GrVkCommandBuffer.cpp
        src/gpu/ganesh/vk/GrVkCommandBuffer.h
        src/gpu/ganesh/vk/GrVkCommandPool.cpp
        src/gpu/ganesh/vk/GrVkCommandPool.h
        src/gpu/ganesh/vk/GrVkDescriptorPool.cpp
        src/gpu/ganesh/vk/GrVkDescriptorPool.h
        src/gpu/ganesh/vk/GrVkDescriptorSet.cpp
        src/gpu/ganesh/vk/GrVkDescriptorSet.h
        src/gpu/ganesh/vk/GrVkDescriptorSetManager.cpp
        src/gpu/ganesh/vk/GrVkDescriptorSetManager.h
        src/gpu/ganesh/vk/GrVkDirectContext.cpp
        src/gpu/ganesh/vk/GrVkFramebuffer.cpp
        src/gpu/ganesh/vk/GrVkFramebuffer.h
        src/gpu/ganesh/vk/GrVkGpu.cpp
        src/gpu/ganesh/vk/GrVkGpu.h
        src/gpu/ganesh/vk/GrVkImage.cpp
        src/gpu/ganesh/vk/GrVkImage.h
        src/gpu/ganesh/vk/GrVkImageLayout.h
        src/gpu/ganesh/vk/GrVkImageView.cpp
        src/gpu/ganesh/vk/GrVkImageView.h
        src/gpu/ganesh/vk/GrVkManagedResource.h
        src/gpu/ganesh/vk/GrVkMSAALoadManager.cpp
        src/gpu/ganesh/vk/GrVkMSAALoadManager.h
        src/gpu/ganesh/vk/GrVkOpsRenderPass.cpp
        src/gpu/ganesh/vk/GrVkOpsRenderPass.h
        src/gpu/ganesh/vk/GrVkPipeline.cpp
        src/gpu/ganesh/vk/GrVkPipeline.h
        src/gpu/ganesh/vk/GrVkPipelineStateBuilder.cpp
        src/gpu/ganesh/vk/GrVkPipelineStateBuilder.h
        src/gpu/ganesh/vk/GrVkPipelineStateCache.cpp
        src/gpu/ganesh/vk/GrVkPipelineState.cpp
        src/gpu/ganesh/vk/GrVkPipelineStateDataManager.cpp
        src/gpu/ganesh/vk/GrVkPipelineStateDataManager.h
        src/gpu/ganesh/vk/GrVkPipelineState.h
        src/gpu/ganesh/vk/GrVkRenderPass.cpp
        src/gpu/ganesh/vk/GrVkRenderPass.h
        src/gpu/ganesh/vk/GrVkRenderTarget.cpp
        src/gpu/ganesh/vk/GrVkRenderTarget.h
        src/gpu/ganesh/vk/GrVkResourceProvider.cpp
        src/gpu/ganesh/vk/GrVkResourceProvider.h
        src/gpu/ganesh/vk/GrVkSampler.cpp
        src/gpu/ganesh/vk/GrVkSampler.h
        src/gpu/ganesh/vk/GrVkSamplerYcbcrConversion.cpp
        src/gpu/ganesh/vk/GrVkSamplerYcbcrConversion.h
        src/gpu/ganesh/vk/GrVkSemaphore.cpp
        src/gpu/ganesh/vk/GrVkSemaphore.h
        src/gpu/ganesh/vk/GrVkTexture.cpp
        src/gpu/ganesh/vk/GrVkTexture.h
        src/gpu/ganesh/vk/GrVkTextureRenderTarget.cpp
        src/gpu/ganesh/vk/GrVkTextureRenderTarget.h
        src/gpu/ganesh/vk/GrVkTypesPriv.cpp
        src/gpu/ganesh/vk/GrVkTypesPriv.h
        src/gpu/ganesh/vk/GrVkUniformHandler.cpp
        src/gpu/ganesh/vk/GrVkUniformHandler.h
        src/gpu/ganesh/vk/GrVkUtil.cpp
        src/gpu/ganesh/vk/GrVkUtil.h
        src/gpu/ganesh/vk/GrVkVaryingHandler.cpp
        src/gpu/ganesh/vk/GrVkVaryingHandler.h
        src/gpu/vk/VulkanExtensions.cpp
        src/gpu/vk/VulkanInterface.cpp
        src/gpu/vk/VulkanInterface.h
        src/gpu/vk/VulkanMemory.cpp
        src/gpu/vk/VulkanMemory.h
        src/gpu/vk/VulkanUtilsPriv.h)

#################################################################################
### DEFINES
#################################################################################
#UNIX_DEFINES = [
#PNG_SKIP_SETJMP_CHECK
#SK_BUILD_FOR_UNIX
#SK_CODEC_DECODES_PNG
#SK_CODEC_DECODES_WEBP
#SK_R32_SHIFT=16
#SK_GL
#SK_CODEC_DECODES_JPEG
#]
#ANDROID_DEFINES = [
#SK_BUILD_FOR_ANDROID
#SK_CODEC_DECODES_PNG
#SK_CODEC_DECODES_WEBP
#SK_GL
#SK_CODEC_DECODES_JPEG
#]
#IOS_DEFINES = [
#SK_BUILD_FOR_IOS
#SK_CODEC_DECODES_JPEG
#]
#WASM_DEFINES = [
#SK_DISABLE_LEGACY_SHADERCONTEXT
#SK_DISABLE_TRACING
#SK_GL
#SK_FORCE_AAA
#SK_DISABLE_EFFECT_DESERIALIZATION
#SK_FORCE_8_BYTE_ALIGNMENT
#SKNX_NO_SIMD
#SK_CODEC_DECODES_JPEG
#]
#FUCHSIA_DEFINES = [
#SK_BUILD_FOR_UNIX
#SK_CODEC_DECODES_PNG
#SK_CODEC_DECODES_WEBP
#SK_R32_SHIFT=16
#SK_VULKAN
#SK_CODEC_DECODES_JPEG
#]
#MACOS_DEFINES = [
#SK_BUILD_FOR_MAC
#SK_GL
#SK_CODEC_DECODES_JPEG
#]
#ANDROID_NO_CODECS_DEFINES = [
#SK_BUILD_FOR_ANDROID
#SK_GL
#]


#############################
set(SKIA_SRC_FILES
        src/codec/SkPixmapUtils.cpp
        ${CODEC_SRCS_ALL}
        ${ENCODE_SRCS}
        ${ENCODE_PNG_SRCS}
        ${NO_ENCODE_JPEG_SRCS}
        ${NO_ENCODE_WEBP_SRCS})
if (XGD_USE_VULKAN)
    list(APPEND SKIA_SRC_FILES ${VULKAN_SRCS})
endif ()
if (WIN32)
    list(APPEND SKIA_SRC_FILES ${PORTS_SRCS_WIN})
elseif (ANDROID)
    list(APPEND SKIA_SRC_FILES
            # ${PORTS_SRCS_ANDROID} # no expat.h found
            ${PORTS_SRCS_ANDROID_NO_FONT})
elseif (IOS)
    list(APPEND SKIA_SRC_FILES ${PORTS_SRCS_IOS} ${GL_SRCS_IOS})
elseif (APPLE)
    list(APPEND SKIA_SRC_FILES ${PORTS_SRCS_MACOS} ${GL_SRCS_MACOS})
elseif (EMSCRIPTEN)
    list(APPEND SKIA_SRC_FILES ${PORTS_SRCS_WASM} ${GL_SRCS_WASM})
elseif (UNIX)
    if (XGD_USE_FONTCONFIG)
        find_package(Fontconfig REQUIRED)
    endif ()
    list(APPEND SKIA_SRC_FILES ${PORTS_SRCS_UNIX} ${GL_SRCS_UNIX})
endif ()

set(SKIA_SRC_DIRS
        ${SRC_DIR}/base
        ${SRC_DIR}/core
        ${SRC_DIR}/effects
        ${SRC_DIR}/effects/colorfilters
        ${SRC_DIR}/effects/imagefilters
        ${SRC_DIR}/fonts
        ${SRC_DIR}/gpu
        ${SRC_DIR}/gpu/ganesh
        ${SRC_DIR}/gpu/ganesh/effects
        ${SRC_DIR}/gpu/ganesh/geometry
        ${SRC_DIR}/gpu/ganesh/glsl
        ${SRC_DIR}/gpu/ganesh/gradients
        ${SRC_DIR}/gpu/ganesh/image
        ${SRC_DIR}/gpu/ganesh/mock
        ${SRC_DIR}/gpu/ganesh/ops
        ${SRC_DIR}/gpu/ganesh/surface
        ${SRC_DIR}/gpu/ganesh/tessellate
        ${SRC_DIR}/gpu/ganesh/text
        ${SRC_DIR}/gpu/tessellate
        ${SRC_DIR}/image
        ${SRC_DIR}/opts
        ${SRC_DIR}/pathops
        ${SRC_DIR}/pdf
        ${SRC_DIR}/sfnt
        ${SRC_DIR}/shaders
        ${SRC_DIR}/shaders/gradients
        ${SRC_DIR}/sksl
        ${SRC_DIR}/sksl/analysis
        ${SRC_DIR}/sksl/codegen
        ${SRC_DIR}/sksl/ir
        ${SRC_DIR}/sksl/tracing
        ${SRC_DIR}/sksl/transform
        ${SRC_DIR}/text
        ${SRC_DIR}/text/gpu
        ${SRC_DIR}/utils
        #        ${SRC_DIR}/xps
)
if (WIN32)
    list(APPEND SKIA_SRC_DIRS ${SRC_DIR}/utils/win)
elseif (APPLE)
    list(APPEND SKIA_SRC_DIRS ${SRC_DIR}/utils/apple)
endif ()

set(SKIA_SRC_FILE_PATHS)
foreach (SKIA_SRC_FILE ${SKIA_SRC_FILES})
    list(APPEND SKIA_SRC_FILE_PATHS ${ROOT_DIR}/${SKIA_SRC_FILE})
endforeach ()

xgd_add_library(skcms STATIC SRC_FILES
        ${ROOT_DIR}/modules/skcms/skcms.cc
        ${ROOT_DIR}/modules/skcms/src/skcms_TransformBaseline.cc
        ${ROOT_DIR}/modules/skcms/src/skcms_TransformHsw.cc
        ${ROOT_DIR}/modules/skcms/src/skcms_TransformSkx.cc
)

xgd_add_library(skia
        SRC_FILES
        ${SKIA_SRC_FILE_PATHS}
        EXCLUDE_SRC_FILES
        ${SRC_DIR}/pdf/SkJpegInfo_libjpegturbo.cpp
        ${SRC_DIR}/pdf/SkDocument_PDF_None.cpp
        ${SRC_DIR}/gpu/ganesh/surface/SkSurface_GaneshMtl.mm
        ${SRC_DIR}/codec/SkWuffsCodec.cpp
        ${SRC_DIR}/codec/SkWebpCodec.cpp
        ${SRC_DIR}/codec/SkJpegCodec.cpp
        ${SRC_DIR}/codec/SkJpegDecoderMgr.cpp
        ${SRC_DIR}/codec/SkJpegMultiPicture.cpp
        ${SRC_DIR}/codec/SkJpegSegmentScan.cpp
        ${SRC_DIR}/codec/SkJpegSourceMgr.cpp
        ${SRC_DIR}/codec/SkJpegUtility.cpp
        ${SRC_DIR}/codec/SkJpegxlCodec.cpp
        ${SRC_DIR}/codec/SkJpegXmp.cpp
        ${SRC_DIR}/utils/win/SkWGL_win.cpp
        SRC_DIRS
        ${SKIA_SRC_DIRS}
        INCLUDE_DIRS
        ${ROOT_DIR}
        ${INC_DIR}/core
        ${INC_DIR}/utils
        ${INC_DIR}/effects
        ${INC_DIR}/encode
        ${INC_DIR}/images)
if (BUILD_SHARED_LIBS)
    target_compile_definitions(skia PUBLIC SKIA_DLL PRIVATE SKIA_IMPLEMENTATION)
endif ()
if (XGD_USE_VULKAN)
    xgd_link_vulkan(skia)
endif ()
if (APPLE)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation REQUIRED)
    find_library(CORETEXT_LIBRARY CoreText REQUIRED)
    find_library(COREGRAPHICS_LIBRARY CoreGraphics REQUIRED)
    find_library(IMAGEIO_LIBRARY ImageIO REQUIRED)
    target_link_libraries(
            skia PRIVATE
            ${COREFOUNDATION_LIBRARY}
            ${CORETEXT_LIBRARY}
            ${COREGRAPHICS_LIBRARY}
            ${IMAGEIO_LIBRARY}
    )
endif ()
target_compile_definitions(skia PRIVATE
        SK_CODEC_DECODES_PNG
        # Our legacy G3 rule *always* has the ganesh backend enabled.
        SK_GANESH
        # Chrome DEFINES.
        SK_USE_FREETYPE_EMBOLDEN
        # Turn on a few Google3-specific build fixes.
        # SK_BUILD_FOR_GOOGLE3
        # Required for building dm.
        GR_TEST_UTILS
        # Should remove after we update golden images
        SK_WEBP_ENCODER_USE_DEFAULT_METHOD
        # Experiment to diagnose image diffs in Google3
        SK_DISABLE_LOWP_RASTER_PIPELINE)
xgd_link_libraries(skia PRIVATE skcms png zlib freetype)
target_link_libraries(skia PRIVATE $<$<BOOL:${ANDROID}>:log>)
if (XGD_USE_FONTCONFIG AND UNIX AND NOT ANDROID AND NOT IOS AND NOT EMSCRIPTEN AND NOT APPLE)
    xgd_link_libraries(skia PRIVATE Fontconfig::Fontconfig)
endif ()

function(xgd_build_skia_module_libs MODULE_NAME)
    target_sources(skia PRIVATE ${ROOT_DIR}/modules/${MODULE_NAME}/src)
    target_include_directories(
            skia
            PUBLIC
            ${ROOT_DIR}
            ${ROOT_DIR}/modules/${MODULE_NAME}/include
    )
    add_library(skia_${MODULE_NAME} ALIAS skia)
    #    xgd_add_library(skia_${MODULE_NAME} STATIC
    #            SRC_DIRS
    #            ${ROOT_DIR}/modules/${MODULE_NAME}/src
    #            INCLUDE_DIRS
    #            ${ROOT_DIR}
    #            ${ROOT_DIR}/modules/${MODULE_NAME}/include)
    #    xgd_link_libraries(skia_${MODULE_NAME} PRIVATE skia)
    #    if (BUILD_SHARED_LIBS)
    #        target_compile_definitions(
    #                skia_${MODULE_NAME} PRIVATE SKIA_IMPLEMENTATION)
    #    endif ()
endfunction()

xgd_build_skia_module_libs(sksg)
#xgd_build_skia_module_libs(skparagraph)
xgd_build_skia_module_libs(skresources)
#xgd_build_skia_module_libs(skottie)
#xgd_build_skia_module_libs(skottie_ios)
xgd_build_skia_module_libs(svg)
#xgd_build_skia_module_libs(xml)
