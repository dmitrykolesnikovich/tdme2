#include <string>
#include <vector>

using std::string;
using std::vector;

#include <tdme/tdme.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingProgram.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingShader.h>
#include <tdme/engine/Engine.h>

using tdme::engine::subsystems::postprocessing::PostProcessingProgram;
using tdme::engine::subsystems::postprocessing::PostProcessingShader;


PostProcessingProgram::PostProcessingProgram(RenderPass renderPass) {
	this->renderPass = renderPass;
}

void PostProcessingProgram::addEffectPass(
	int effectPassIdx,
	int frameBufferWidthDivideFactor,
	int frameBufferHeightDivideFactor,
	string shaderPrefix,
	bool useEZR,
	bool applyShadowMapping,
	bool applyPostProcessing,
	int32_t renderTypes,
	Color4 clearColor,
	bool renderLightSources,
	bool skipOnLightSourceNotVisible
) {
	PostProcessingProgramEffectPass effectPass;
	effectPass.effectPassIdx = effectPassIdx;
	effectPass.frameBufferWidthDivideFactor = frameBufferWidthDivideFactor;
	effectPass.frameBufferHeightDivideFactor = frameBufferHeightDivideFactor;
	effectPass.shaderPrefix = shaderPrefix;
	effectPass.useEZR = useEZR;
	effectPass.applyShadowMapping = applyShadowMapping;
	effectPass.applyPostProcessing = applyPostProcessing;
	effectPass.renderTypes = renderTypes;
	effectPass.clearColor = clearColor;
	effectPass.renderLightSources = renderLightSources;
	effectPass.skipOnLightSourceNotVisible = skipOnLightSourceNotVisible;
	//effectPass
	effectPasses.push_back(effectPass);
}

void PostProcessingProgram::addPostProcessingStep(string shaderId, PostProcessingProgram::FrameBufferSource source, PostProcessingProgram::FrameBufferTarget target, bool bindTemporary, PostProcessingProgram::FrameBufferSource blendToSource) {
	PostProcessingProgramStep step;
	step.shaderId = shaderId;
	step.source = source;
	step.target = target;
	step.bindTemporary = bindTemporary;
	step.blendToSource = blendToSource;
	steps.push_back(step);
}

bool PostProcessingProgram::isSupported() {
	for (auto step: steps) {
		if (Engine::getInstance()->getPostProcessingShader()->hasShader(step.shaderId) == false) return false;
	}
	return true;

}
