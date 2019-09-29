#pragma once
#include <iostream>

#ifdef _MSC_VER
#define PACK(__Declaration__)                                                  \
  __pragma(pack(push, 1)) __Declaration__ __pragma(pack(pop))
#elif defined(__GNUC__)
#define PACK(__Declaration__) __Declaration__ __attribute__((__packed__))
#else
#error Unsupported platform
#endif

typedef float float32_t;

enum ObjClass : uint8_t { car = 1, truck = 0 };

enum Movement : uint8_t { turnLeft = 2, turnRight = 3, straight = 1 };

PACK(struct ObjProperty
{
    enum ObjClass objClass = ObjClass::car;
    float32_t width = 0.0;
    float32_t length = 0.0;
});

PACK(struct HistTraceInfo
{
    int32_t currFrameIndex = 0;
    enum Movement movement = Movement::straight;
    int32_t state = 0;
    float32_t startLong = 0.0;
    float32_t startLat = 0.0;
});

PACK(struct CurrInfo
{
    float32_t distLong = 0.0;
    float32_t distLat = 0.0;
    float32_t velLat = 0.0;
    float32_t velLong = 0.0;
    float32_t acc = 0.0;
});

PACK(struct FrameItemData
{
    int32_t objId = 0;
    ObjProperty objProperty;
    HistTraceInfo histTraceInfo;
    CurrInfo currInfo;
});

PACK(struct FrameStructData
{
    // timeStamp int64
    int32_t radarID;
    int32_t length;
    int64_t timeStamp;
    FrameItemData frameData[256];
    // objID, distLong, distLat, velocityLong, velocityLat, state
});

PACK(struct MagicHeader
{
    int32_t Number;
    int32_t PackageLength;
});

PACK(struct ScaleSetInfo
{
    int32_t index;
    float32_t max;
    float32_t min;
});
