#include "LongBow.h"
#include "BowRenderDeviceManager.h"
#include "IBowGraphicsWindow.h"
#include "BowVertexAttributeFloatVec3.h"

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
    vert[0] =-0.3f; vert[1] = 0.5f; vert[2] =-1.0f;
    vert[3] =-0.8f; vert[4] =-0.5f; vert[5] =-1.0f;
    vert[6] = 0.2f; vert[7] =-0.5f; vert[8]= -1.0f;

	// create VertexArray
	auto VertexArray = Context->CreateVertexArray();

	// fill buffer with informations
	auto	VertexBufferAttribute = Bow::VertexBufferAttributePtr( new Bow::VertexBufferAttribute( Device->VCreateVertexBuffer(Bow::BufferHint::StaticDraw, sizeof(float) * 9), Bow::ComponentDatatype::Float, 3 ) );
			VertexBufferAttribute->GetVertexBuffer()->CopyFromSystemMemory(vert, 0, sizeof(float) * 9);

	// connect buffer with location
	VertexArray->SetAttribute(ShaderProgram->GetVertexAttribute("in_Position")->Location, VertexBufferAttribute);

	///////////////////////////////////////////////////////////////////
	// Uniforms

	float rosa[] = {1.0f, 0.0f, 1.0f};
	ShaderProgram->GetUniform("u_color")->SetArray(rosa, false);

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