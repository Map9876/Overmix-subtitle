#ifndef WEBGPU_H_STUB
#define WEBGPU_H_STUB

// Stub header for building without WebGPU support
#include <cstdint>
#include <cstddef>

typedef void* WGPUDevice;
typedef void* WGPUBuffer;
typedef void* WGPUInstance;
typedef void* WGPUAdapter;
typedef void* WGPUQueue;
typedef void* WGPUShaderModule;
typedef void* WGPUComputePipeline;
typedef void* WGPUBindGroup;
typedef void* WGPUBindGroupLayout;
typedef void* WGPUCommandEncoder;
typedef void* WGPUCommandBuffer;
typedef void* WGPUComputePassEncoder;
typedef void* WGPUPipelineLayout;
typedef void* WGPUSampler;
typedef void* WGPUTextureView;
typedef void* WGPUQuerySet;

// Use enum (not enum class) for operator| support
enum WGPUBufferUsage : uint32_t {
    WGPUBufferUsage_None = 0,
    WGPUBufferUsage_MapRead = 0x0001,
    WGPUBufferUsage_MapWrite = 0x0002,
    WGPUBufferUsage_CopySrc = 0x0004,
    WGPUBufferUsage_CopyDst = 0x0008,
    WGPUBufferUsage_Index = 0x0010,
    WGPUBufferUsage_Vertex = 0x0020,
    WGPUBufferUsage_Uniform = 0x0040,
    WGPUBufferUsage_Storage = 0x0080,
    WGPUBufferUsage_Indirect = 0x0100,
    WGPUBufferUsage_QueryResolve = 0x0200,
};

typedef uint32_t WGPUShaderStage;
static constexpr WGPUShaderStage WGPUShaderStage_None = 0;
static constexpr WGPUShaderStage WGPUShaderStage_Vertex = 1;
static constexpr WGPUShaderStage WGPUShaderStage_Fragment = 2;
static constexpr WGPUShaderStage WGPUShaderStage_Compute = 4;

typedef uint32_t WGPUBufferBindingType;
static constexpr WGPUBufferBindingType WGPUBufferBindingType_Undefined = 0x0000;
static constexpr WGPUBufferBindingType WGPUBufferBindingType_Uniform = 0x0001;
static constexpr WGPUBufferBindingType WGPUBufferBindingType_Storage = 0x0002;
static constexpr WGPUBufferBindingType WGPUBufferBindingType_ReadOnlyStorage = 0x0003;

typedef uint32_t WGPUSamplerBindingType;
static constexpr WGPUSamplerBindingType WGPUSamplerBindingType_Undefined = 0x0000;
static constexpr WGPUSamplerBindingType WGPUSamplerBindingType_Filtering = 0x0001;
static constexpr WGPUSamplerBindingType WGPUSamplerBindingType_NonFiltering = 0x0002;
static constexpr WGPUSamplerBindingType WGPUSamplerBindingType_Comparison = 0x0003;

typedef uint32_t WGPUStorageTextureAccess;
static constexpr WGPUStorageTextureAccess WGPUStorageTextureAccess_Undefined = 0x0000;
static constexpr WGPUStorageTextureAccess WGPUStorageTextureAccess_WriteOnly = 0x0001;
static constexpr WGPUStorageTextureAccess WGPUStorageTextureAccess_ReadOnly = 0x0002;
static constexpr WGPUStorageTextureAccess WGPUStorageTextureAccess_ReadWrite = 0x0003;

typedef uint32_t WGPUTextureFormat;
static constexpr WGPUTextureFormat WGPUTextureFormat_Undefined = 0;

typedef uint32_t WGPUTextureViewDimension;
static constexpr WGPUTextureViewDimension WGPUTextureViewDimension_Undefined = 0;

typedef uint32_t WGPUTextureSampleType;
static constexpr WGPUTextureSampleType WGPUTextureSampleType_Undefined = 0;

typedef uint32_t WGPUMapMode;
static constexpr WGPUMapMode WGPUMapMode_Read = 1;
static constexpr WGPUMapMode WGPUMapMode_Write = 2;

typedef uint32_t WGPUSType;
static constexpr WGPUSType WGPUSType_ShaderModuleWGSLDescriptor = 0x0006;

typedef uint32_t WGPURequestAdapterStatus;
static constexpr WGPURequestAdapterStatus WGPURequestAdapterStatus_Success = 0;

typedef uint32_t WGPURequestDeviceStatus;
static constexpr WGPURequestDeviceStatus WGPURequestDeviceStatus_Success = 0;

typedef uint32_t WGPUBufferMapAsyncStatus;
static constexpr WGPUBufferMapAsyncStatus WGPUBufferMapAsyncStatus_Success = 0;

typedef uint32_t WGPUQueueWorkDoneStatus;
static constexpr WGPUQueueWorkDoneStatus WGPUQueueWorkDoneStatus_Success = 0;

typedef uint32_t WGPUErrorType;

// Struct stubs
struct WGPUChainedStruct {
    void* next = nullptr;
    WGPUSType sType = {};
};

struct WGPUInstanceDescriptor {
    WGPUChainedStruct* nextInChain = nullptr;
};

struct WGPURequestAdapterOptions {};

struct WGPULimits {
    uint32_t maxTextureDimension1D = 0;
    uint32_t maxTextureDimension2D = 0;
    uint32_t maxTextureDimension3D = 0;
    uint32_t maxTextureArrayLayers = 0;
    uint32_t maxBindGroups = 0;
    uint32_t maxBindingsPerBindGroup = 0;
    uint32_t maxDynamicUniformBuffersPerPipelineLayout = 0;
    uint32_t maxDynamicStorageBuffersPerPipelineLayout = 0;
    uint32_t maxSampledTexturesPerShaderStage = 0;
    uint32_t maxSamplersPerShaderStage = 0;
    uint32_t maxStorageBuffersPerShaderStage = 0;
    uint32_t maxStorageTexturesPerShaderStage = 0;
    uint32_t maxUniformBuffersPerShaderStage = 0;
    uint64_t maxUniformBufferBindingSize = 0;
    uint64_t maxStorageBufferBindingSize = 0;
    uint32_t minUniformBufferOffsetAlignment = 256;
    uint32_t minStorageBufferOffsetAlignment = 256;
    uint32_t maxVertexBuffers = 0;
    uint64_t maxBufferSize = 0;
    uint32_t maxVertexAttributes = 0;
    uint32_t maxVertexBufferArrayStride = 0;
    uint32_t maxInterStageShaderComponents = 0;
    uint32_t maxInterStageShaderVariables = 0;
    uint32_t maxColorAttachments = 0;
    uint32_t maxColorAttachmentBytesPerSample = 0;
    uint32_t maxComputeWorkgroupStorageSize = 0;
    uint32_t maxComputeInvocationsPerWorkgroup = 0;
    uint32_t maxComputeWorkgroupSizeX = 0;
    uint32_t maxComputeWorkgroupSizeY = 0;
    uint32_t maxComputeWorkgroupSizeZ = 0;
    uint32_t maxComputeWorkgroupsPerDimension = 0;
};

struct WGPUSupportedLimits {
    WGPUChainedStruct* nextInChain = nullptr;
    WGPULimits limits = {};
};

struct WGPURequiredLimits {
    WGPUChainedStruct* nextInChain = nullptr;
    WGPULimits limits = {};
};

struct WGPUQueueDescriptor {
    WGPUChainedStruct* nextInChain = nullptr;
    const char* label = nullptr;
};

struct WGPUDeviceDescriptor {
    WGPUChainedStruct* nextInChain = nullptr;
    const char* label = nullptr;
    uint32_t requiredFeaturesCount = 0;
    void* requiredFeatures = nullptr;
    WGPURequiredLimits* requiredLimits = nullptr;
    WGPUQueueDescriptor defaultQueue = {};
    void* deviceLostCallback = nullptr;
    void* deviceLostUserdata = nullptr;
};

struct WGPUShaderModuleDescriptor {
    WGPUChainedStruct* nextInChain = nullptr;
    const char* label = nullptr;
};

struct WGPUShaderModuleWGSLDescriptor {
    WGPUChainedStruct chain = {};
    const char* code = nullptr;
};

struct WGPUProgrammableStageDescriptor {
    WGPUShaderModule module = nullptr;
    const char* entryPoint = nullptr;
    uint32_t constantCount = 0;
    void* constants = nullptr;
};

struct WGPUComputePipelineDescriptor {
    WGPUChainedStruct* nextInChain = nullptr;
    const char* label = nullptr;
    WGPUPipelineLayout layout = nullptr;
    WGPUProgrammableStageDescriptor compute = {};
};

struct WGPUPipelineLayoutDescriptor {
    WGPUChainedStruct* nextInChain = nullptr;
    const char* label = nullptr;
    uint32_t bindGroupLayoutCount = 0;
    WGPUBindGroupLayout* bindGroupLayouts = nullptr;
    uint32_t constantCount = 0;
    void* constants = nullptr;
};

struct WGPUBufferBindingLayout {
    WGPUChainedStruct* nextInChain = nullptr;
    WGPUBufferBindingType type = {};
    int hasDynamicOffset = 0;
    uint64_t minBindingSize = 0;
};

struct WGPUSamplerBindingLayout {
    WGPUChainedStruct* nextInChain = nullptr;
    WGPUSamplerBindingType type = {};
};

struct WGPUStorageTextureBindingLayout {
    WGPUChainedStruct* nextInChain = nullptr;
    WGPUStorageTextureAccess access = {};
    WGPUTextureFormat format = {};
    WGPUTextureViewDimension viewDimension = {};
};

struct WGPUTextureBindingLayout {
    WGPUChainedStruct* nextInChain = nullptr;
    WGPUTextureSampleType sampleType = {};
    int multisampled = 0;
    WGPUTextureViewDimension viewDimension = {};
};

struct WGPUBindGroupLayoutEntry {
    WGPUChainedStruct* nextInChain = nullptr;
    uint32_t binding = 0;
    WGPUShaderStage visibility = 0;
    WGPUBufferBindingLayout buffer = {};
    WGPUSamplerBindingLayout sampler = {};
    WGPUStorageTextureBindingLayout storageTexture = {};
    WGPUTextureBindingLayout texture = {};
};

struct WGPUBindGroupLayoutDescriptor {
    WGPUChainedStruct* nextInChain = nullptr;
    const char* label = nullptr;
    uint32_t entryCount = 0;
    WGPUBindGroupLayoutEntry* entries = nullptr;
};

struct WGPUBindGroupEntry {
    WGPUChainedStruct* nextInChain = nullptr;
    uint32_t binding = 0;
    WGPUBuffer buffer = nullptr;
    uint64_t offset = 0;
    uint64_t size = 0;
    WGPUSampler sampler = nullptr;
    WGPUTextureView textureView = nullptr;
};

struct WGPUBindGroupDescriptor {
    WGPUChainedStruct* nextInChain = nullptr;
    const char* label = nullptr;
    WGPUBindGroupLayout layout = nullptr;
    uint32_t entryCount = 0;
    WGPUBindGroupEntry* entries = nullptr;
};

struct WGPUBufferDescriptor {
    WGPUChainedStruct* nextInChain = nullptr;
    const char* label = nullptr;
    WGPUBufferUsage usage = WGPUBufferUsage_None;
    uint64_t size = 0;
    int mappedAtCreation = 0;
};

struct WGPUCommandEncoderDescriptor {
    WGPUChainedStruct* nextInChain = nullptr;
    const char* label = nullptr;
};

struct WGPUCommandBufferDescriptor {
    WGPUChainedStruct* nextInChain = nullptr;
    const char* label = nullptr;
};

struct WGPUComputePassTimestampWrites {
    WGPUQuerySet querySet = nullptr;
    uint32_t beginningOfPassWriteIndex = 0;
    uint32_t endOfPassWriteIndex = 0;
};

struct WGPUComputePassDescriptor {
    WGPUChainedStruct* nextInChain = nullptr;
    const char* label = nullptr;
    WGPUComputePassTimestampWrites* timestampWrites = nullptr;
};

// Function stubs
inline WGPUInstance wgpuCreateInstance(const WGPUInstanceDescriptor*) { return nullptr; }
inline void wgpuInstanceRelease(WGPUInstance) {}
inline void wgpuInstanceRequestAdapter(WGPUInstance, const WGPURequestAdapterOptions*, void(*)(WGPURequestAdapterStatus, WGPUAdapter, const char*, void*), void*) {}
inline void wgpuAdapterRelease(WGPUAdapter) {}
inline void wgpuAdapterRequestDevice(WGPUAdapter, const WGPUDeviceDescriptor*, void(*)(WGPURequestDeviceStatus, WGPUDevice, const char*, void*), void*) {}
inline void wgpuDeviceRelease(WGPUDevice) {}
inline WGPUQueue wgpuDeviceGetQueue(WGPUDevice) { return nullptr; }
inline void wgpuQueueRelease(WGPUQueue) {}
inline void wgpuQueueOnSubmittedWorkDone(WGPUQueue, void(*)(WGPUQueueWorkDoneStatus, void*), void*) {}
inline WGPUShaderModule wgpuDeviceCreateShaderModule(WGPUDevice, const WGPUShaderModuleDescriptor*) { return nullptr; }
inline void wgpuShaderModuleRelease(WGPUShaderModule) {}
inline WGPUComputePipeline wgpuDeviceCreateComputePipeline(WGPUDevice, const WGPUComputePipelineDescriptor*) { return nullptr; }
inline void wgpuComputePipelineRelease(WGPUComputePipeline) {}
inline WGPUPipelineLayout wgpuDeviceCreatePipelineLayout(WGPUDevice, const WGPUPipelineLayoutDescriptor*) { return nullptr; }
inline void wgpuPipelineLayoutRelease(WGPUPipelineLayout) {}
inline WGPUBindGroupLayout wgpuDeviceCreateBindGroupLayout(WGPUDevice, const WGPUBindGroupLayoutDescriptor*) { return nullptr; }
inline void wgpuBindGroupLayoutRelease(WGPUBindGroupLayout) {}
inline WGPUBindGroup wgpuDeviceCreateBindGroup(WGPUDevice, const WGPUBindGroupDescriptor*) { return nullptr; }
inline void wgpuBindGroupRelease(WGPUBindGroup) {}
inline WGPUBuffer wgpuDeviceCreateBuffer(WGPUDevice, const WGPUBufferDescriptor*) { return nullptr; }
inline void wgpuBufferDestroy(WGPUBuffer) {}
inline void wgpuBufferMapAsync(WGPUBuffer, WGPUMapMode, size_t, size_t, void(*)(WGPUBufferMapAsyncStatus, void*), void*) {}
inline void* wgpuBufferGetMappedRange(WGPUBuffer, size_t, size_t) { return nullptr; }
inline const void* wgpuBufferGetConstMappedRange(WGPUBuffer, size_t, size_t) { return nullptr; }
inline void wgpuBufferUnmap(WGPUBuffer) {}
inline WGPUCommandEncoder wgpuDeviceCreateCommandEncoder(WGPUDevice, const WGPUCommandEncoderDescriptor*) { return nullptr; }
inline void wgpuCommandEncoderRelease(WGPUCommandEncoder) {}
inline void wgpuCommandEncoderInsertDebugMarker(WGPUCommandEncoder, const char*) {}
inline void wgpuCommandEncoderCopyBufferToBuffer(WGPUCommandEncoder, WGPUBuffer, uint64_t, WGPUBuffer, uint64_t, uint64_t) {}
inline WGPUComputePassEncoder wgpuCommandEncoderBeginComputePass(WGPUCommandEncoder, const WGPUComputePassDescriptor*) { return nullptr; }
inline void wgpuComputePassEncoderRelease(WGPUComputePassEncoder) {}
inline void wgpuComputePassEncoderSetPipeline(WGPUComputePassEncoder, WGPUComputePipeline) {}
inline void wgpuComputePassEncoderSetBindGroup(WGPUComputePassEncoder, uint32_t, WGPUBindGroup, size_t, const uint32_t*) {}
inline void wgpuComputePassEncoderDispatchWorkgroups(WGPUComputePassEncoder, uint32_t, uint32_t, uint32_t) {}
inline void wgpuComputePassEncoderEnd(WGPUComputePassEncoder) {}
inline WGPUCommandBuffer wgpuCommandEncoderFinish(WGPUCommandEncoder, const WGPUCommandBufferDescriptor*) { return nullptr; }
inline void wgpuCommandBufferRelease(WGPUCommandBuffer) {}
inline void wgpuQueueSubmit(WGPUQueue, uint32_t, const WGPUCommandBuffer*) {}
inline void wgpuQueueWriteBuffer(WGPUQueue, WGPUBuffer, uint64_t, const void*, uint64_t) {}
inline void wgpuDeviceSetUncapturedErrorCallback(WGPUDevice, void(*)(WGPUErrorType, const char*, void*), void*) {}
inline void wgpuInstanceProcessEvents(WGPUInstance) {}
inline void wgpuAdapterGetLimits(WGPUAdapter, WGPUSupportedLimits*) {}

#endif
