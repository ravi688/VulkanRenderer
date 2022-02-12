#include <Math/Math.hpp>
#include <Math/Vec3.hpp>
#include <Math/Vec4.hpp>
#include <Math/Mat4.hpp>

#include <Renderer/Renderer.hpp>
#include <Renderer/TextMesh.hpp>
#include <Renderer/Mesh.hpp>
#include <Renderer/Material.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/Texture.hpp>
#include <Renderer/RenderWindow.hpp>

#include <Renderer/Mesh3D.hpp>
#include <Showcase/Time.hpp>

static Math::Mat4 projectionMatrix;
static Math::Mat4 screenSpaceMatrix;

static void recreateMatrices(V3D::RenderWindow window, void* user_data)
{
	projectionMatrix = Math::Mat4::perspectiveProjection(0.04f, 100, 60 * DEG2RAD, (float)window.getWidth()/window.getHeight());
	screenSpaceMatrix = Math::Mat4::orthographicProjection(-0.04f, 100, window.getHeight(), (float)window.getWidth()/window.getHeight());
}

static void u32_to_string(u32 value, char* string)
{
	sprintf(string, "%u", value);
}

int main(int argc, char** argv)
{
	V3D::Renderer myRenderer(V3D::RendererGPUType::DISCRETE);
	myRenderer.init();

	recreateMatrices(myRenderer.getRenderWindow(), NULL);
	myRenderer.getRenderWindow().subscribeOnResize(recreateMatrices, NULL);

	Math::Mat4 cameraTransform = Math::Mat4::transform({ -1.8f, 0.6f, 0 }, { 0, 0, -22 * DEG2RAD });
	Math::Mat4 viewMatrix = cameraTransform.inverse();
	Math::Mat4 clipMatrix = Math::Mat4::identity(); clipMatrix.m11 = -1;

	/*------TEXT-----------------------------*/
	V3D::Font font("resource/fonts/Pushster-Regular.ttf");
	V3D::GlyphMeshPool pool = myRenderer.createGlyphMeshPool(font);
	V3D::Mesh glyph_A = pool.getMesh('A');
	V3D::Mesh glyph_B = pool.getMesh('B');
	V3D::TextMesh textMesh = myRenderer.createTextMesh(pool);
	
	V3D::TextMesh::StringHandle fpsString = textMesh.createString();
	textMesh.setStringPosition(fpsString, { 0, 0, -3 });
	
	V3D::TextMesh::StringHandle moduleNameString = textMesh.createString();
	textMesh.setStringPosition(moduleNameString, { 0, 2, -3 });
	textMesh.setStringScale(moduleNameString, { 1.5f, 1.5f, 1.5f });
	textMesh.setString(moduleNameString, "Hello World");
	
	V3D::TextMesh::StringHandle versionNameString = textMesh.createString();
	textMesh.setStringPosition(versionNameString, { 0, -2, -3 });
	textMesh.setStringScale(versionNameString, { 0.7f, 0.7f, 0.7f });
	textMesh.setString(versionNameString, "v1.0.0.0.0");
	
	V3D::TextMesh::StringHandle cpu_usage = textMesh.createString();
	textMesh.setStringPosition(cpu_usage, { 0, -4, -3 });
	textMesh.setStringScale(cpu_usage, { 0.7f, 0.7f, 0.7f });
	textMesh.setString(cpu_usage, "10.99999%");

	V3D::Shader textShader = myRenderer.loadShader("resource/shaders/text_shader.sb");
	V3D::Material textMaterial = myRenderer.createMaterial(textShader);
	/*---------------------------------------*/

	/*-----------GAME UI----------------------*/
	V3D::Shader gameUIShader = myRenderer.loadShader("resource/shaders/game_ui_shader.sb");
	V3D::Material gameUIMaterial = myRenderer.createMaterial(gameUIShader);

	V3D::TextMesh gameUI = myRenderer.createTextMesh(pool);
	V3D::TextMesh::StringHandle scoreString = gameUI.createString();
	gameUI.setStringPosition(scoreString, { 0, 7, 5 });
	gameUI.setStringScale(scoreString, { 0.8f, 0.8f, 0.8f });

	/*--------PLAYGROUND----------------------*/
	V3D::Mesh3D rockMesh3d = V3D::Mesh3D::load("resource/Rock/Rock.obj");
	rockMesh3d.makeCentroidOrigin();
	rockMesh3d.calculateTangents();
	V3D::Mesh rock = myRenderer.createMesh(rockMesh3d);
	rockMesh3d.drop();

	V3D::Shader rockShader = myRenderer.loadShader("resource/shaders/bump_shader.sb");
	V3D::Material rockMaterial = myRenderer.createMaterial(rockShader);
	V3D::Texture rockTextures[] = 
	{ 
		myRenderer.loadTexture(V3D::TextureType::ALBEDO, "resource/Rock/albedo.bmp"),
		myRenderer.loadTexture(V3D::TextureType::NORMAL, "resource/Rock/normal.bmp")
	};
	rockMaterial.set("albedo", rockTextures[0]);
	rockMaterial.set("normal_map", rockTextures[1]);
	/*---------------------------------------*/

	/*----------- CUBEMAP - SKYBOX ----------------------*/
	V3D::Texture skyboxTexture = myRenderer.loadTexture(V3D::TextureType::CUBE,
											"resource/skybox_textures/skybox/right.bmp",
											"resource/skybox_textures/skybox/left.bmp",
											"resource/skybox_textures/skybox/bottom.bmp",
											"resource/skybox_textures/skybox/top.bmp",
											"resource/skybox_textures/skybox/front.bmp", 
											"resource/skybox_textures/skybox/back.bmp");
	V3D::Shader skyboxShader = myRenderer.loadShader("resource/shaders/skybox_shader.sb");
	V3D::Material skyboxMaterial = myRenderer.createMaterial(skyboxShader);
	skyboxMaterial.set("skybox", skyboxTexture);
	
	V3D::Mesh3D skyboxMesh3d = V3D::Mesh3D::cube(5);
	skyboxMesh3d.flipTriangles();
	V3D::Mesh skybox = myRenderer.createMesh(skyboxMesh3d);
	skyboxMesh3d.drop();

	/*----------- QUAD ----------------------*/
	V3D::Mesh3D quadMesh3d = V3D::Mesh3D::plane(0.6f);
	V3D::Mesh quad = myRenderer.createMesh(quadMesh3d);
	quadMesh3d.drop();

	V3D::Shader quadShader = myRenderer.loadShader("resource/shaders/transparent_shader.sb");
	V3D::Material quadMaterial = myRenderer.createMaterial(quadShader);

	Showcase::Time::Handle frameTimeHandle = Showcase::Time::createHandle();
	Showcase::Time::Handle secondsTimeHandle = Showcase::Time::createHandle();
	Showcase::Time::Handle gameTimeHandle = Showcase::Time::createHandle();
	u32 frameCount = 0;
	float angle = 0;
	u32 textureIndex = 0;
	//TODO: render loop should run on separate thread -> render thread
	while(myRenderer.isRunning())
	{
		float delta_time = Showcase::Time::getDeltaTime(frameTimeHandle);
		float game_time = Showcase::Time::getSeconds(gameTimeHandle);
		textMaterial.set("ubo.time", game_time);
		gameUIMaterial.set("ubo.time", game_time);

		auto rotation_matrix = Math::Mat4::rotation(0, angle * DEG2RAD, 0);
		auto light_dir = rotation_matrix * Math::Vec4::forward();

		rockMaterial.set("light.color", Math::Vec3 { 1, 1, 1 });
		rockMaterial.set("light.dir",  Math::Vec3(light_dir.x, light_dir.y, light_dir.z).normalize());
		rockMaterial.set("light.intensity", 4.0f);

		myRenderer.beginFrame(0.01f, 0.1f, 0.2f, 0.4f);

		rockMaterial.bind();
		Math::Mat4 model_matrix = Math::Mat4::rotation(0, 0 * DEG2RAD, 0) * Math::Mat4::scale(2.2, 2.2, 2.2);
		Math::Mat4 mvp = clipMatrix * projectionMatrix * viewMatrix * model_matrix;
		rockMaterial.setPush("push.mvp_matrix", mvp.transpose());
		rockMaterial.setPush("push.model_matrix", model_matrix.transpose());
		rock.drawIndexed();

		skyboxMaterial.bind();

		Math::Mat4 skybox_matrix = viewMatrix;
		// skybox should remain at origin, so set the last column of the viewMatrix to zero
		float** elements = (float**)skybox_matrix.getData();
		elements[0][3] = 0;
		elements[1][3] = 0;
		elements[2][3] = 0;
		elements[3][3] = 1;
		mvp = clipMatrix * projectionMatrix * skybox_matrix;
		skyboxMaterial.setPush("push.mvp_matrix", mvp.transpose());
		skybox.draw();

		quadMaterial.bind();
		model_matrix = Math::Mat4::translation(-0.8f, 0, 0) * Math::Mat4::rotation(0, 0, 80 * DEG2RAD);
		mvp = clipMatrix * projectionMatrix * viewMatrix * model_matrix;
		quadMaterial.setPush("push.mvp_matrix", mvp.transpose());
		quad.draw();

		textMaterial.bind();
		Math::Mat4 canvas_transform = clipMatrix * screenSpaceMatrix;
		Math::Mat4 _model_matrix = Math::Mat4::translation(0, 0, 0) * Math::Mat4::scale(0, 50, 50);
		textMaterial.setPush("push.mvp_matrix", (canvas_transform * _model_matrix).transpose());
		textMesh.draw();

		gameUIMaterial.bind();
		_model_matrix = Math::Mat4::scale(50, 50, 50) * Math::Mat4::identity();
		gameUIMaterial.setPush("push.mvp_matrix", (canvas_transform * _model_matrix).transpose());
		gameUI.draw();
		
		myRenderer.endFrame();
		myRenderer.update();

		angle += 180 * delta_time * 0.1f;
		if(angle >= 360.0f)
			angle = 0.0f;

		float seconds = Showcase::Time::getSeconds(secondsTimeHandle);
		if(seconds >= 0.05f)
		{
			u32 fps = (u32)((int)frameCount / seconds);
			char fps_string[32];
			strcpy(fps_string, "FPS: ");
			u32_to_string(fps, fps_string + strlen("FPS: "));
			textMesh.setString(fpsString, fps_string);
			strcpy(fps_string, "Score: ");
			u32_to_string(frameCount, fps_string + strlen("Score: "));
			gameUI.setString(scoreString, fps_string);
			secondsTimeHandle = Showcase::Time::createHandle();
			frameCount = 0;
			textureIndex++;
			textureIndex = textureIndex % 3;
		}
		++frameCount;
	}

	font.drop();

	pool.drop();
	gameUI.drop();
	textMesh.drop();
	rock.drop();
	quad.drop();
	skybox.drop();
	
	textShader.drop();
	gameUIShader.drop();
	rockShader.drop();
	quadShader.drop();
	skyboxShader.drop();
	
	textMaterial.drop();
	gameUIMaterial.drop();
	rockMaterial.drop();
	quadMaterial.drop();
	skyboxMaterial.drop();

	for(int i = 0; i < 2; i++)
		rockTextures[i].drop();

	myRenderer.terminate();
	return 0;
}
