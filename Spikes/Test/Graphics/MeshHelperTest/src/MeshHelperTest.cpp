#include <gtest/gtest.h>

#include <Core/Types.hpp>
#include <Math/Vector.hpp>
#include <Helper/MeshHelper.hpp>

// 必要なヘッダーをインクルード
using namespace ts;
using namespace ts::kit;

TEST(MeshHelperTest, CalculateTangents_FlatQuad)
{
    // 1. モックデータの構築（Z=0の平面に置いた四角形を構成する2つの三角形）
    std::vector<Vector3> positions = 
    {
        { 0.0f, 1.0f, 0.0f }, // 0: 左上
        { 1.0f, 1.0f, 0.0f }, // 1: 右上
        { 0.0f, 0.0f, 0.0f }, // 2: 左下
        { 1.0f, 0.0f, 0.0f }  // 3: 右下
    };

    // UV座標（一般的な左上0,0の系）
    std::vector<Vector2> uvs = 
    {
        { 0.0f, 0.0f }, // 0: 左上
        { 1.0f, 0.0f }, // 1: 右上
        { 0.0f, 1.0f }, // 2: 左下
        { 1.0f, 1.0f }  // 3: 右下
    };

    // 法線はすべて手前（-Z方向）を向いているとする
    std::vector<Vector3> normals = 
    {
        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, -1.0f }
    };

    std::vector<u32> indices = 
    {
        0, 1, 2, // 三角形1
        2, 1, 3  // 三角形2
    };

    // 2. テスト対象関数の実行
    kit::meshhelper::ComputeTangentsAndBitangentsParameters parameters = {};
    parameters.m_positions = positions;
    parameters.m_normals = normals;
    parameters.m_uvs = uvs;
    parameters.m_indicesU32 = indices;

    std::vector<Vector4> outTangents = {};
    outTangents.resize(parameters.m_positions.size());
    bool succeeded = kit::meshhelper::ComputeTangentsAndBitangents(
        parameters,
        outTangents
    );

    // 3. アサート（期待値の検証）
    EXPECT_TRUE(succeeded);
     
    // 平面の場合、Tangent（U方向）はX軸プラス(1, 0, 0)、W成分は 1.0f (または -1.0f) になるはず！
    EXPECT_EQ(outTangents.size(), 4);
    for (const auto& t : outTangents) 
    {
        EXPECT_NEAR(t.x, 1.0f, 1e-5f);
        EXPECT_NEAR(t.y, 0.0f, 1e-5f);
        EXPECT_NEAR(t.z, 0.0f, 1e-5f);
        EXPECT_FLOAT_EQ(t.w, 1.0f);
    }
}