#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/shadowmapping/fwd-tdme.h>
#include <tdme/math/Matrix4x4.h>

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;
using tdme::math::Matrix4x4;

/**
 * Shadow mapping shader interface to render shadow map
 * @author Andreas Drewke
 */
struct tdme::engine::subsystems::shadowmapping::ShadowMapRenderShaderImplementation
{
	/**
	 * Destructor
	 */
	virtual ~ShadowMapRenderShaderImplementation() {}

	/**
	 * @return shader id
	 */
	virtual const string getId() = 0;

	/**
	 * @return if initialized and ready to use
	 */
	virtual bool isInitialized() = 0;

	/**
	 * Init shadow mapping
	 */
	virtual void initialize() = 0;

	/**
	 * Use shadow map render shader program
	 * @param engine engine
	 * @param contextIdx context index
	 */
	virtual void useProgram(Engine* engine, int contextIdx) = 0;

	/**
	 * Un use shadow map render shader program
	 * @param contextIdx context index
	 */
	virtual void unUseProgram(int contextIdx) = 0;

	/**
	 * Update matrices
	 * @param contextIdx context index
	 */
	virtual void updateMatrices(int contextIdx) = 0;

	/**
	 * Update texture matrix
	 * @param renderer renderer
	 * @param context contet
	 */
	virtual void updateTextureMatrix(Renderer* renderer, int contextIdx) = 0;

	/**
	 * Update material
	 * @param renderer renderer
	 * @param context contet
	 */
	virtual void updateMaterial(Renderer* renderer, int contextIdx) = 0;

	/**
	 * Update light
	 * @param renderer renderer
	 * @param context contet
	 * @param lightId light id
	 */
	virtual void updateLight(Renderer* renderer, int contextIdx, int32_t lightId) = 0;

	/**
	 * Update shader parameters
	 * @param renderer renderer
	 * @param contextIdx context index
	 */
	virtual void updateShaderParameters(Renderer* renderer, int contextIdx) = 0;

	/**
	 * Bind texture
	 * @param renderer renderer
	 * @param context contet
	 * @param textureId texture id
	 */
	virtual void bindTexture(Renderer* renderer, int contextIdx, int32_t textureId) = 0;

	/**
	 * Set up program depth bias mvp matrix
	 * @param context contet
	 * @param depthBiasMVPMatrix depth bias mvp matrix
	 */
	virtual void setDepthBiasMVPMatrix(int contextIdx, const Matrix4x4& depthBiasMVPMatrix) = 0;

	/**
	 * Set light id
	 * @param lightId light id to render
	 */
	virtual void setRenderLightId(int32_t lightId) = 0;
};
