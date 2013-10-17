#pragma once
#include "LongBow.h"
#include "BowTexture2DDescription.h"
#include "IBowWritePixelBuffer.h"
#include "IBowReadPixelBuffer.h"

namespace Bow {

	enum ImageDatatype : char
    {
        Byte,
        UnsignedByte,
        Short,
        UnsignedShort,
        Int,
        UnsignedInt,
        Float,
        HalfFloat,
        UnsignedByte332,
        UnsignedShort4444,
        UnsignedShort5551,
        UnsignedInt8888,
        UnsignedInt1010102,
        UnsignedByte233Reversed,
        UnsignedShort565,
        UnsignedShort565Reversed,
        UnsignedShort4444Reversed,
        UnsignedShort1555Reversed,
        UnsignedInt8888Reversed,
        UnsignedInt2101010Reversed,
        UnsignedInt248,
        UnsignedInt10F11F11FReversed,
        UnsignedInt5999Reversed,
        Float32UnsignedInt248Reversed
    };

	enum ImageFormat : char
    {
        StencilIndex,
        DepthComponent,
        Red,
        Green,
        Blue,
        RedGreenBlue,
        RedGreenBlueAlpha,
        BlueGreenRed,
        BlueGreenRedAlpha,
        RedGreen,
        RedGreenInteger,
        DepthStencil,
        RedInteger,
        GreenInteger,
        BlueInteger,
        RedGreenBlueInteger,
        RedGreenBlueAlphaInteger,
        BlueGreenRedInteger,
        BlueGreenRedAlphaInteger
    };

	class ITexture2D
	{
	public:

		virtual void CopyFromBuffer( WritePixelBufferPtr pixelBuffer, ImageFormat format, ImageDatatype dataType, int rowAlignment = 4)
        {
            CopyFromBuffer(pixelBuffer, 0, 0, GetDescription().GetWidth(), GetDescription().GetHeight(), format, dataType, rowAlignment);
        }

		virtual void CopyFromBuffer( WritePixelBufferPtr pixelBuffer, int xOffset, int yOffset, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment) = 0;

		virtual void CopyFromSystemMemory(void* bitmapInSystemMemory, ImageFormat format, ImageDatatype dataType, int rowAlignment = 4)
        {
			CopyFromSystemMemory(bitmapInSystemMemory, 0, 0, GetDescription().GetWidth(), GetDescription().GetHeight(), format, dataType, rowAlignment);
        }

        virtual void CopyFromSystemMemory(void* bitmapInSystemMemory, int xOffset, int yOffset, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment) = 0;
        
		virtual VoidPtr CopyToSystemMemory( ImageFormat format, ImageDatatype dataType, int rowAlignment = 4) = 0;

        virtual Texture2DDescription GetDescription() = 0;
	};

	typedef std::shared_ptr<ITexture2D> Texture2DPtr;
}