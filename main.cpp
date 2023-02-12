#pragma comment(lib,"DxErr9")
#pragma comment(lib,"d3d9")
#pragma comment(lib,"d3dx9")
#pragma comment(lib,"d3dx9d")
#pragma comment(lib,"winmm")

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include<windows.h>
#include<mmsystem.h>
#include<d3d9.h>
#include<d3dx9.h>

TCHAR szClassName[]=TEXT("Window");
HWND                    g_hWnd=0;
LPDIRECT3D9             g_pD3D=0;
LPDIRECT3DDEVICE9       g_pd3dDevice=0;
LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer=0;
LPDIRECT3DTEXTURE9      g_pTexture=0;

float  g_fElapsedTime;
double g_dCurrentTime;
double g_dLastTime;

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)

struct Vertex
{
    float x,y,z;
    float tu,tv;
};

Vertex g_cubeVertices[]=
{
	{-1.0f,1.0f,-1.0f,0.0f,0.0f },
	{ 1.0f,1.0f,-1.0f,1.0f,0.0f },
	{-1.0f,-1.0f,-1.0f,0.0f,1.0f },
	{ 1.0f,-1.0f,-1.0f,1.0f,1.0f },
	
	{-1.0f,1.0f,1.0f,1.0f,0.0f },
	{-1.0f,-1.0f,1.0f,1.0f,1.0f },
	{ 1.0f,1.0f,1.0f,0.0f,0.0f },
	{ 1.0f,-1.0f,1.0f,0.0f,1.0f },
	
	{-1.0f,1.0f,1.0f,0.0f,0.0f },
	{ 1.0f,1.0f,1.0f,1.0f,0.0f },
	{-1.0f,1.0f,-1.0f,0.0f,1.0f },
	{ 1.0f,1.0f,-1.0f,1.0f,1.0f },
	
	{-1.0f,-1.0f,1.0f,0.0f,0.0f },
	{-1.0f,-1.0f,-1.0f,1.0f,0.0f },
	{ 1.0f,-1.0f,1.0f,0.0f,1.0f },
	{ 1.0f,-1.0f,-1.0f,1.0f,1.0f },

	{ 1.0f,1.0f,-1.0f,0.0f,0.0f },
	{ 1.0f,1.0f,1.0f,1.0f,0.0f },
	{ 1.0f,-1.0f,-1.0f,0.0f,1.0f },
	{ 1.0f,-1.0f,1.0f,1.0f,1.0f },
	
	{-1.0f,1.0f,-1.0f,1.0f,0.0f },
	{-1.0f,-1.0f,-1.0f,1.0f,1.0f },
	{-1.0f,1.0f,1.0f,0.0f,0.0f },
	{-1.0f,-1.0f,1.0f,0.0f,1.0f }
};

void loadTexture(void)	
{
    D3DXCreateTextureFromFile(g_pd3dDevice,"sample.jpg",&g_pTexture);
	g_pd3dDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
    g_pd3dDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
}

void init(void)
{
    g_pD3D=Direct3DCreate9(D3D_SDK_VERSION);
    D3DDISPLAYMODE d3ddm;
    g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3ddm);
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp,sizeof(d3dpp));
    d3dpp.Windowed=TRUE;
    d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat=d3ddm.Format;
    d3dpp.EnableAutoDepthStencil=TRUE;
    d3dpp.AutoDepthStencilFormat=D3DFMT_D16;
    d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
    g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,g_hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp,&g_pd3dDevice);
	loadTexture();
	g_pd3dDevice->CreateVertexBuffer(24*sizeof(Vertex),0,D3DFVF_CUSTOMVERTEX,D3DPOOL_DEFAULT,&g_pVertexBuffer,0);
	void*pVertices=0;
    g_pVertexBuffer->Lock(0,sizeof(g_cubeVertices),(void**)&pVertices,0);
    memcpy(pVertices,g_cubeVertices,sizeof(g_cubeVertices));
    g_pVertexBuffer->Unlock();
    g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH(&matProj,D3DXToRadian(45.0f),640.0f/480.0f,0.1f,100.0f);
    g_pd3dDevice->SetTransform(D3DTS_PROJECTION,&matProj);
}

void shutDown(void)
{
    if(g_pTexture)g_pTexture->Release();
    if(g_pVertexBuffer)g_pVertexBuffer->Release(); 
    if(g_pd3dDevice)g_pd3dDevice->Release();
    if(g_pD3D)g_pD3D->Release();
}

void render(void)
{
    g_pd3dDevice->Clear(0,0,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),1.0f,0);
    static float fXrot=0.0f;
	static float fYrot=0.0f;
	static float fZrot=0.0f;
	fXrot+=10.1f*g_fElapsedTime;
	fYrot+=10.2f*g_fElapsedTime;
	fZrot+=10.3f*g_fElapsedTime;
    D3DXMATRIX matWorld;
    D3DXMATRIX matTrans;
	D3DXMATRIX matRot;
    D3DXMatrixTranslation(&matTrans,0.0f,0.0f,5.0f);
	D3DXMatrixRotationYawPitchRoll(&matRot,D3DXToRadian(fXrot),D3DXToRadian(fYrot),D3DXToRadian(fZrot));
    matWorld=matRot*matTrans;
    g_pd3dDevice->SetTransform(D3DTS_WORLD,&matWorld);
    g_pd3dDevice->BeginScene();
    g_pd3dDevice->SetTexture(0,g_pTexture);
    g_pd3dDevice->SetStreamSource(0,g_pVertexBuffer,0,sizeof(Vertex));
    g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,4,2);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,8,2);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,12,2);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,16,2);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,20,2);
    g_pd3dDevice->EndScene();
    g_pd3dDevice->Present(0,0,0,0);
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch(msg)
	{	
        case WM_DESTROY:
            PostQuitMessage(0);
	        break;
		default:
			return DefWindowProc(hWnd,msg,wParam,lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE,LPSTR,int nCmdShow)
{
	MSG msg;
    memset(&msg,0,sizeof msg);
	WNDCLASS wndclass={0,WndProc,0,0,hInstance,0,LoadCursor(0,IDC_ARROW),0,0,szClassName};
	RegisterClass(&wndclass);
	g_hWnd=CreateWindow(szClassName,TEXT("Window"),WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,0,CW_USEDEFAULT,0,0,0,hInstance,0);
	ShowWindow(g_hWnd,nCmdShow);
	UpdateWindow(g_hWnd);
	init();
	while(msg.message!=WM_QUIT)
	{
		if(PeekMessage(&msg,0,0,0,PM_REMOVE))
		{ 
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
        else
        {
            g_dCurrentTime=timeGetTime();
            g_fElapsedTime=(float)((g_dCurrentTime-g_dLastTime)*0.001);
            g_dLastTime=g_dCurrentTime;
            render();
        }
	}
	shutDown();
	return msg.wParam;
}
