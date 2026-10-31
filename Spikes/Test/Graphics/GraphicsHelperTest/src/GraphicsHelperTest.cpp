#include <gtest/gtest.h>

#include <Helper/GraphicsHelper.hpp>

using namespace ts;
using namespace ts::kit;

TEST(GraphicsHelperTest, F32ToU8SDR)
{
	// 変換テスト
	{
		const Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

		const u8 r = graphicshelper::F32ToU8SDR(color.x);
		const u8 g = graphicshelper::F32ToU8SDR(color.y);
		const u8 b = graphicshelper::F32ToU8SDR(color.z);
		const u8 a = graphicshelper::F32ToU8SDR(color.w);

		EXPECT_EQ(r, 255);
		EXPECT_EQ(g, 255);
		EXPECT_EQ(b, 255);
		EXPECT_EQ(a, 255);
	}

	// オーバーフロー/アンダーフローテスト
	{
		// オーバーフロー
		{
			const Vector4 overflowColor = { 10.0f, 10.0f, 10.0f, 10.0f };

			const u8 clampedRed = graphicshelper::F32ToU8SDR(overflowColor.x);
			const u8 clampedGreen = graphicshelper::F32ToU8SDR(overflowColor.y);
			const u8 clampedBlue = graphicshelper::F32ToU8SDR(overflowColor.z);
			const u8 clampedAlpha = graphicshelper::F32ToU8SDR(overflowColor.w);

			EXPECT_EQ(clampedRed, 255);
			EXPECT_EQ(clampedGreen, 255);
			EXPECT_EQ(clampedBlue, 255);
			EXPECT_EQ(clampedAlpha, 255);
		}

		// アンダーフロー
		{
			const Vector4 underflowColor = { -10.0f, -10.0f, -10.0f, -10.0f };

			const u8 clampedRed = graphicshelper::F32ToU8SDR(underflowColor.x);
			const u8 clampedGreen = graphicshelper::F32ToU8SDR(underflowColor.y);
			const u8 clampedBlue = graphicshelper::F32ToU8SDR(underflowColor.z);
			const u8 clampedAlpha = graphicshelper::F32ToU8SDR(underflowColor.w);

			EXPECT_EQ(clampedRed, 0);
			EXPECT_EQ(clampedGreen, 0);
			EXPECT_EQ(clampedBlue, 0);
			EXPECT_EQ(clampedAlpha, 0);
		}
	}
}

TEST(GraphicsHelperTest, PackUnpackColor)
{
	// Vector4 -> u32
	{
		// White
		{
			const Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

			const u32 packedColor = graphicshelper::PackColor(color);

			EXPECT_EQ(packedColor, 0xFFFFFFFF);
		}
		
		// Black
		{
			const Vector4 color = { 0.0f, 0.0f, 0.0f, 1.0f };

			const u32 packedColor = graphicshelper::PackColor(color);

			EXPECT_EQ(packedColor, 0xFF000000u);
		}
	}

	// u32 -> Vector4
	{
		// White
		{
			const u32 white = 0xFFFFFFFF;

			const Vector4 whiteColor = graphicshelper::UnpackColor(white);

			EXPECT_NEAR(whiteColor.x, 1.0f, 1.0e-5f);
			EXPECT_NEAR(whiteColor.y, 1.0f, 1.0e-5f);
			EXPECT_NEAR(whiteColor.z, 1.0f, 1.0e-5f);
			EXPECT_NEAR(whiteColor.w, 1.0f, 1.0e-5f);
		}

		// Black
		{
			const u32 black = 0xFF000000;

			const Vector4 blackColor = graphicshelper::UnpackColor(black);

			EXPECT_NEAR(blackColor.x, 0.0f, 1.0e-5f);
			EXPECT_NEAR(blackColor.y, 0.0f, 1.0e-5f);
			EXPECT_NEAR(blackColor.z, 0.0f, 1.0e-5f);
			EXPECT_NEAR(blackColor.w, 1.0f, 1.0e-5f);
		}
	}
}
