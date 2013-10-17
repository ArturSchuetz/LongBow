#include "LongBow.h"
#include "BowRenderDeviceManager.h"
#include "IBowGraphicsWindow.h"
#include "BowVertexAttributeFloatVec3.h"

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
	auto Window			= Device->VCreateWindow();
	auto Context		= Window->VGetContext();
	auto ShaderProgram	= Device->VCreateShaderProgram(LoadShader(IDS_VERTEXSHADER), LoadShader(IDS_FRAGMENTSHADER));

	///////////////////////////////////////////////////////////////////
	// ClearState and Color

	Bow::ClearState	clearState;
	float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	memcpy(&clearState.color, &color, sizeof(float)*4);

	///////////////////////////////////////////////////////////////////
	// Vertex Array

	float* vert = new float[9];     // vertex array 
    vert[0] = -1.0f; vert[1] =  1.0f; vert[2] = 1.0f;
    vert[3] =  0.0f; vert[4] = -1.0f; vert[5] = 1.0f;
    vert[6] =  1.0f; vert[7] =  1.0f; vert[8]=  1.0f;

	
	float* texcoor = new float[6];     // vertex array 
    texcoor[0] = 0.0f; texcoor[1] = 1.0f;
	texcoor[2] = 0.5f; texcoor[3] = 0.0f;
	texcoor[4] = 1.0f; texcoor[5] = 1.0f;

	// create VertexArray
	auto VertexArray = Context->CreateVertexArray();

	// fill buffer with informations
	auto	PositionAttribute = Bow::VertexBufferAttributePtr( new Bow::VertexBufferAttribute( Device->VCreateVertexBuffer(Bow::BufferHint::StaticDraw, sizeof(float) * 9), Bow::ComponentDatatype::Float, 3 ));
	PositionAttribute->GetVertexBuffer()->CopyFromSystemMemory(vert, 0, sizeof(float) * 9);
	
	auto	TextureCoordAttribute = Bow::VertexBufferAttributePtr( new Bow::VertexBufferAttribute( Device->VCreateVertexBuffer(Bow::BufferHint::StaticDraw, sizeof(float) * 6), Bow::ComponentDatatype::Float, 2 ));
	TextureCoordAttribute->GetVertexBuffer()->CopyFromSystemMemory(texcoor, 0, sizeof(float) * 6);

	// connect buffer with location
	VertexArray->SetAttribute(ShaderProgram->GetVertexAttribute("in_Position")->Location, PositionAttribute);
	VertexArray->SetAttribute(ShaderProgram->GetVertexAttribute("in_TexCoord")->Location, TextureCoordAttribute);

	///////////////////////////////////////////////////////////////////
	// Textures

	auto texture = Device->VCreateTexture2DFromFile("Data/Textures/Logo_Art_wip_6.jpg");
	auto sampler = Device->VCreateTexture2DSampler(Bow::TextureMinificationFilter::Linear, Bow::TextureMagnificationFilter::Linear, Bow::TextureWrap::Clamp, Bow::TextureWrap::Clamp);

	int TexID = 0;
	Context->VSetTexture(TexID, texture);
	Context->VSetTextureSampler(TexID, sampler);
	ShaderProgram->GetUniform("diffuseTex")->SetValue(&TexID, false);

	///////////////////////////////////////////////////////////////////
	// RenderState

	Bow::RenderState renderState;
    renderState.FacetCulling.Enabled = false;
    renderState.DepthTest.Enabled = false;
	
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
			Context->VMakeCurrent();
			Context->VClear(clearState);

			Context->VDraw( Bow::PrimitiveType::Triangles, 0, 3, VertexArray, ShaderProgram, renderState );

			Context->VSwapBuffers();
        }
	}
	Bow::LOG_INIT("Quiting Game");
	return (int) msg.wParam;
}