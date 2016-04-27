#include "Engine/Renderer/AnimationMotion.hpp"
#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Input/BinaryReader.hpp"
#include "Engine/Input/BinaryWriter.hpp"

//-----------------------------------------------------------------------------------
AnimationMotion::AnimationMotion(const std::string& motionName, float timeSpan, float framerate, Skeleton* skeleton)
	: m_motionName(motionName)
	, m_frameCount(ceil(framerate * timeSpan) + 1)
	, m_jointCount(skeleton->GetJointCount())
	, m_totalLengthSeconds(timeSpan)
	, m_frameTime(1.0f/framerate)
	, m_frameRate(framerate)
	, m_keyframes(new Matrix4x4[m_frameCount * m_jointCount])
{
}

//-----------------------------------------------------------------------------------
AnimationMotion::~AnimationMotion()
{
	delete[] m_keyframes;
}

//-----------------------------------------------------------------------------------
void AnimationMotion::GetFrameIndicesWithBlend(uint32_t& outFrameIndex0, uint32_t& outFrameIndex1, float& outBlend, float inTime)
{
	uint32_t frameIndex0 = (uint32_t)floor(inTime / m_frameTime);
	uint32_t frameIndex1 = frameIndex0 + 1;

	if (frameIndex0 == (m_frameCount - 1))
	{
		frameIndex1 = m_frameCount - 1;
		outBlend = 0.0f;
	}
	else if (frameIndex0 == (m_frameCount - 2))
	{
		float lastFrameTime = m_totalLengthSeconds - (m_frameTime * frameIndex0);
		outBlend = fmodf(inTime, m_frameTime) / lastFrameTime;
		outBlend = MathUtils::Clamp(outBlend, 0.0f, 1.0f);
	}
	else
	{
		outBlend = fmodf(inTime, m_frameTime) / m_frameTime;
	}

	outFrameIndex0 = frameIndex0;
	outFrameIndex1 = frameIndex1;
}

//-----------------------------------------------------------------------------------
Matrix4x4* AnimationMotion::GetJointKeyframes(uint32_t jointIndex)
{
	return m_keyframes + (m_frameCount * jointIndex);
}

//-----------------------------------------------------------------------------------
void AnimationMotion::ApplyMotionToSkeleton(Skeleton* skeleton, float time)
{
	uint32_t frame0 = 0;
	uint32_t frame1 = 0;
	float blend;
	GetFrameIndicesWithBlend(frame0, frame1, blend, time);

	uint32_t jointCount = skeleton->GetJointCount();
	for (uint32_t jointIndex = 0; jointIndex < jointCount; ++jointIndex)
	{
		Matrix4x4* jointKeyframes = GetJointKeyframes(jointIndex);
		Matrix4x4& matrix0 = jointKeyframes[frame0];
		Matrix4x4& matrix1 = jointKeyframes[frame1];

		Matrix4x4 newModel = Matrix4x4::MatrixLerp(matrix0, matrix1, blend);

		//Needs to set bone to model matrix
		//(Or set your matrix tree's world to this, and set
		//bone to model on Skelelton world's array
		skeleton->m_boneToModelSpace[jointIndex] = newModel; //SetJointWorldTransform(jointIndex, newModel);
	}
}


//-----------------------------------------------------------------------------------
void AnimationMotion::WriteToFile(const char* filename)
{
    BinaryFileWriter writer;
    ASSERT_OR_DIE(writer.Open(filename), "File Open failed!");
    {
        WriteToStream(writer);
    }
    writer.Close();
}

//-----------------------------------------------------------------------------------
void AnimationMotion::WriteToStream(IBinaryWriter& writer)
{
    //FILE VERSION
    //Frame Count
    //Total Length Seconds
    //Framerate
    //Frametime
    //Motion name
    //Joint count
    //Keyframes

    writer.Write<uint32_t>(FILE_VERSION);
    writer.Write<uint32_t>(m_frameCount);
    writer.Write<float>(m_totalLengthSeconds);
    writer.Write<float>(m_frameRate);
    writer.Write<float>(m_frameTime);
    writer.WriteString(m_motionName.c_str());
    writer.Write<int>(m_jointCount);

    unsigned int numKeyframes = m_frameCount * m_jointCount;
    for (unsigned int index = 0; index < numKeyframes; ++index)
    {
        Matrix4x4 mat = m_keyframes[index];
        for (int i = 0; i < 16; ++i)
        {
            writer.Write<float>(mat.data[i]);
        }
    }
}

//-----------------------------------------------------------------------------------
void AnimationMotion::ReadFromStream(IBinaryReader& reader)
{
    //FILE VERSION
    //Frame Count
    //Total Length Seconds
    //Framerate
    //Frametime
    //Motion name
    //Joint count
    //Keyframes

    uint32_t fileVersion = 0;
    ASSERT_OR_DIE(reader.Read<uint32_t>(fileVersion), "Failed to read file version");
    ASSERT_OR_DIE(fileVersion == FILE_VERSION, "File version didn't match!");
    ASSERT_OR_DIE(reader.Read<uint32_t>(m_frameCount), "Failed to read frame count");
    ASSERT_OR_DIE(reader.Read<float>(m_totalLengthSeconds), "Failed to read frame count");
    ASSERT_OR_DIE(reader.Read<float>(m_frameRate), "Failed to read frame count");
    ASSERT_OR_DIE(reader.Read<float>(m_frameTime), "Failed to read frame count");
    const char* motionName = nullptr;
    reader.ReadString(motionName, 64);
    m_motionName = std::string(motionName);
    ASSERT_OR_DIE(reader.Read<int>(m_jointCount), "Failed to read frame count");

    unsigned int numKeyframes = m_frameCount * m_jointCount; 
    m_keyframes = new Matrix4x4[numKeyframes];
    for (unsigned int index = 0; index < numKeyframes; ++index)
    {
        Matrix4x4 matrix = Matrix4x4::IDENTITY;
        for (int i = 0; i < 16; ++i)
        {
            reader.Read<float>(matrix.data[i]);
        }
        m_keyframes[index] = matrix;
    }
}

//-----------------------------------------------------------------------------------
void AnimationMotion::ReadFromFile(const char* filename)
{
    BinaryFileReader reader;
    ASSERT_OR_DIE(reader.Open(filename), "File Open failed!");
    {
        ReadFromStream(reader);
    }
    reader.Close();
}