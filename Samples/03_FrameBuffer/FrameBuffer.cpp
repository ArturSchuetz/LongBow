#include "BowRenderDeviceManager.h"

#include "BowBitmap.h"

#include "BasicTimer.h"
#include "resource.h"

#define FPS 30.0f

std::string LoadShader(int name)
{
	DWORD size = 0;
    const char* data = NULL;
    HMODULE handle = ::GetModuleHandle(NULL);
    HRSRC rc = ::FindResource(handle, MAKEINTRESOURCE(name), MAKEINTRESOURCE(SHADER));
    HGLOBAL rcData = ::LoadResource(handle, rc);
    size = ::SizeofResource(handle, rc);
	return std::string(static_cast<const char*>(::LockResource(rcData)));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Bow::RenderDeviceManager Manager;
	auto Device			= Manager.GetOrCreateDevice(Bow::API::OPENGL);
	auto Window			= Device->VCreateWindow(600, 600, "FrameBuffer Sample");
	auto Context		= Window->VGetContext();
	
	///////////////////////////////////////////////////////////////////
	// ClearState and Color

	Bow::ClearState	clearState;
	float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	memcpy(&clearState.color, &black, sizeof(float)*4);

	///////////////////////////////////////////////////////////////////
	// Vertex Array and Shader
	
	auto ShaderProgram	= Device->VCreateShaderProgram(LoadShader(IDS_VERTEXSHADER), LoadShader(IDS_FRAGMENTSHADER));

	float* vert = new float[6];     // vertex array 
    vert[0] = -1.0f; vert[1] =  1.0f;
    vert[2] =  0.0f; vert[3] = -1.0f;
    vert[4] =  1.0f; vert[5] =  1.0f;

	float* texcoor = new float[6];     // vertex array 
    texcoor[0] = 0.0f; texcoor[1] = 1.0f;
	texcoor[2] = 0.5f; texcoor[3] = 0.0f;
	texcoor[4] = 1.0f; texcoor[5] = 1.0f;

	// fill buffer with informations
	auto PositionAttribute = Bow::VertexBufferAttributePtr( new Bow::VertexBufferAttribute( Device->VCreateVertexBuffer(Bow::BufferHint::StaticDraw, sizeof(float) * 6), Bow::ComponentDatatype::Float, 2 ));
	PositionAttribute->GetVertexBuffer()->CopyFromSystemMemory( vert, 0, sizeof(float) * 6 );
	
	auto TextureCoordAttribute = Bow::VertexBufferAttributePtr( new Bow::VertexBufferAttribute( Device->VCreateVertexBuffer(Bow::BufferHint::StaticDraw, sizeof(float) * 6), Bow::ComponentDatatype::Float, 2 ));
	TextureCoordAttribute->GetVertexBuffer()->CopyFromSystemMemory( texcoor, 0, sizeof(float) * 6 );

	// connect buffer with location
	auto VertexArray = Context->CreateVertexArray();
	VertexArray->SetAttribute( ShaderProgram->GetVertexAttribute("in_Position")->Location, PositionAttribute );
	VertexArray->SetAttribute( ShaderProgram->GetVertexAttribute("in_TexCoord")->Location, TextureCoordAttribute );
	
	///////////////////////////////////////////////////////////////////
	// Screenfilling Quad Preperations

	auto QuadShaderProgram	= Device->VCreateShaderProgram(LoadShader(IDS_VERTEXSHADER), LoadShader(IDS_FRAGMENTSHADER));

	float* QuadVert = new float[8];     // vertex array 
    QuadVert[0] =-1.0f; QuadVert[1] = 1.0f; 
	QuadVert[2] =-1.0f; QuadVert[3] =-1.0f;
	QuadVert[4] = 1.0f; QuadVert[5] = 1.0f;
	QuadVert[6] = 1.0f; QuadVert[7] = -1.0f;

	float* QuadTexcoor = new float[8];     // vertex array 
    QuadTexcoor[0] = 0.0f; QuadTexcoor[1] = 1.0f;
	QuadTexcoor[2] = 0.0f; QuadTexcoor[3] = 0.0f;
	QuadTexcoor[4] = 1.0f; QuadTexcoor[5] = 1.0f;
	QuadTexcoor[6] = 1.0f; QuadTexcoor[7] = 0.0f;

	// fill buffer with informations
	auto QuadPositionAttribute = Bow::VertexBufferAttributePtr( new Bow::VertexBufferAttribute( Device->VCreateVertexBuffer(Bow::BufferHint::StaticDraw, sizeof(float) * 8), Bow::ComponentDatatype::Float, 2 ));
	QuadPositionAttribute->GetVertexBuffer()->CopyFromSystemMemory( QuadVert, 0, sizeof(float) * 8 );
	
	auto QuadTextureCoordAttribute = Bow::VertexBufferAttributePtr( new Bow::VertexBufferAttribute( Device->VCreateVertexBuffer(Bow::BufferHint::StaticDraw, sizeof(float) * 8), Bow::ComponentDatatype::Float, 2 ));
	QuadTextureCoordAttribute->GetVertexBuffer()->CopyFromSystemMemory( QuadTexcoor, 0, sizeof(float) * 8 );

	// connect buffer with location
	auto QuadVertexArray = Context->CreateVertexArray();
	QuadVertexArray->SetAttribute( QuadShaderProgram->GetVertexAttribute("in_Position")->Location, QuadPositionAttribute );
	QuadVertexArray->SetAttribute( QuadShaderProgram->GetVertexAttribute("in_TexCoord")->Location, QuadTextureCoordAttribute );

	///////////////////////////////////////////////////////////////////
	// Textures

	auto texture = Device->VCreateTexture2DFromFile( "Data/Textures/Logo_Art_wip_6.jpg" );
	auto sampler = Device->VCreateTexture2DSampler( Bow::TextureMinificationFilter::Linear, Bow::TextureMagnificationFilter::Linear, Bow::TextureWrap::MirroredRepeat, Bow::TextureWrap::MirroredRepeat );

	///////////////////////////////////////////////////////////////////
	// FrameBuffer

	auto FrameBuffer = Context->CreateFramebuffer();
	int out_Color_Location = ShaderProgram->GetFragmentOutputLocation("out_Color");

	FrameBuffer->SetColorAttachment(out_Color_Location, Device->VCreateTexture2D(Bow::Texture2DDescription(Window->VGetWidth(), Window->VGetHeight(), Bow::TextureFormat::RedGreenBlue8)) );

	///////////////////////////////////////////////////////////////////
	// RenderState

	Bow::RenderState renderState;
    renderState.FacetCulling.Enabled = false;
    renderState.DepthTest.Enabled = false;
	renderState.StencilTest.Enabled = false;
	
	///////////////////////////////////////////////////////////////////
	// Gameloop

	MSG msg = { 0 };
	Bow::LOG_INIT("Running Game");

	BasicTimer timer;
	while(WM_QUIT != msg.message)
	{
		Bow::LOG_UPDATE();
		timer.Update();
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
			int diffuseTex = 0;
			Context->VMakeCurrent();
			
			// Render Triangle to Framebuffer
			Context->VSetFramebuffer(FrameBuffer);

			Context->VClear(clearState);
			Context->VSetTexture(diffuseTex, texture);
			Context->VSetTextureSampler(diffuseTex, sampler);
			ShaderProgram->GetUniform("diffuseTex")->SetValue(&diffuseTex, false);
			Context->VDraw( Bow::PrimitiveType::Triangles, VertexArray, ShaderProgram, renderState );
			
			// TODO: Render Fullscreen Quad to Backbuffer
			Context->VSetFramebuffer(nullptr);

			Context->VClear(clearState);
			Context->VSetTexture(diffuseTex, FrameBuffer->GetColorAttachment( out_Color_Location ));
			Context->VSetTextureSampler(diffuseTex, sampler);
			QuadShaderProgram->GetUniform("diffuseTex")->SetValue(&diffuseTex, false);
			Context->VDraw( Bow::PrimitiveType::TriangleStrip, QuadVertexArray, QuadShaderProgram, renderState );
			
			Context->VSwapBuffers();
        }
	}
	Bow::LOG_INIT("Quiting Game");
	return (int) msg.wParam;
}