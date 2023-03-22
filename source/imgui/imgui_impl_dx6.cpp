// dear imgui: Renderer Backend for DirectX6

#include "imgui.h"
#include "imgui_impl_dx6.h"

#include "tVertex.h"
#include "d3d/d3dtextr.h"

#include "GBH.h"

static bool init = false;
static tVertex* vertexBuffer = {};
static int vertexBufferSize = 0;
static LPDIRECT3DTEXTURE2 fontTexture = nullptr;

#ifdef IMGUI_USE_BGRA_PACKED_COLOR
#define IMGUI_COL_TO_DX9_ARGB(_COL)     (_COL)
#else
#define IMGUI_COL_TO_DX9_ARGB(_COL)     (((_COL) & 0xFF00FF00) | (((_COL) & 0xFF0000) >> 16) | (((_COL) & 0xFF) << 16))
#endif

// Functions
static int renderStates[D3DRENDERSTATE_CLIPPLANEENABLE];

static void SetState(unsigned int type, void* value) {
    RenderStateGet(type, (void*)&renderStates[type]);
    RenderStateSet(type, (void*)value);
}

static void ImGui_ImplDX6_SetupRenderState()
{
    // Setup render state: fixed-pipeline, alpha-blending, no face culling, no depth testing, shade mode (for gradient), bilinear sampling.
    SetState(D3DRENDERSTATE_FILLMODE, (void*)D3DFILL_SOLID);
    SetState(D3DRENDERSTATE_SHADEMODE, (void*)D3DSHADE_GOURAUD);
    SetState(D3DRENDERSTATE_ZWRITEENABLE, (void*)FALSE);
    SetState(D3DRENDERSTATE_ALPHATESTENABLE, (void*)FALSE);
    SetState(D3DRENDERSTATE_CULLMODE, (void*)D3DCULL_NONE);
    SetState(D3DRENDERSTATE_ZENABLE, (void*)FALSE);
    SetState(D3DRENDERSTATE_ALPHABLENDENABLE, (void*)TRUE);
    SetState(D3DRENDERSTATE_SRCBLEND, (void*)D3DBLEND_SRCALPHA);
    SetState(D3DRENDERSTATE_DESTBLEND, (void*)D3DBLEND_INVSRCALPHA);
    SetState(D3DRENDERSTATE_FOGENABLE, (void*)FALSE);
    SetState(D3DRENDERSTATE_RANGEFOGENABLE, (void*)FALSE);
    SetState(D3DRENDERSTATE_SPECULARENABLE, (void*)FALSE);
    SetState(D3DRENDERSTATE_STENCILENABLE, (void*)FALSE);
    SetState(D3DRENDERSTATE_CLIPPING, (void*)TRUE);
    SetState(D3DRENDERSTATE_LIGHTING, (void*)FALSE);
    SetState(D3DRENDERSTATE_TEXTUREMIN, (void*)D3DFILTER_LINEAR);
    SetState(D3DRENDERSTATE_TEXTUREMAG, (void*)D3DFILTER_LINEAR);
}

static void ImGui_ImplDX6_RestoreRenderState() {
    RenderStateSet(D3DRENDERSTATE_FILLMODE, (void*)renderStates[D3DRENDERSTATE_FILLMODE]);
    RenderStateSet(D3DRENDERSTATE_SHADEMODE, (void*)renderStates[D3DRENDERSTATE_SHADEMODE]);
    RenderStateSet(D3DRENDERSTATE_ZWRITEENABLE, (void*)renderStates[D3DRENDERSTATE_ZWRITEENABLE]);
    RenderStateSet(D3DRENDERSTATE_ALPHATESTENABLE, (void*)renderStates[D3DRENDERSTATE_ALPHATESTENABLE]);
    RenderStateSet(D3DRENDERSTATE_CULLMODE, (void*)renderStates[D3DRENDERSTATE_CULLMODE]);
    RenderStateSet(D3DRENDERSTATE_ZENABLE, (void*)renderStates[D3DRENDERSTATE_ZENABLE]);
    RenderStateSet(D3DRENDERSTATE_ALPHABLENDENABLE, (void*)renderStates[D3DRENDERSTATE_ALPHABLENDENABLE]);
    RenderStateSet(D3DRENDERSTATE_SRCBLEND, (void*)renderStates[D3DRENDERSTATE_SRCBLEND]);
    RenderStateSet(D3DRENDERSTATE_DESTBLEND, (void*)renderStates[D3DRENDERSTATE_DESTBLEND]);
    RenderStateSet(D3DRENDERSTATE_FOGENABLE, (void*)renderStates[D3DRENDERSTATE_FOGENABLE]);
    RenderStateSet(D3DRENDERSTATE_RANGEFOGENABLE, (void*)renderStates[D3DRENDERSTATE_RANGEFOGENABLE]);
    RenderStateSet(D3DRENDERSTATE_SPECULARENABLE, (void*)renderStates[D3DRENDERSTATE_SPECULARENABLE]);
    RenderStateSet(D3DRENDERSTATE_STENCILENABLE, (void*)renderStates[D3DRENDERSTATE_STENCILENABLE]);
    RenderStateSet(D3DRENDERSTATE_CLIPPING, (void*)renderStates[D3DRENDERSTATE_CLIPPING]);
    RenderStateSet(D3DRENDERSTATE_LIGHTING, (void*)renderStates[D3DRENDERSTATE_LIGHTING]);
    RenderStateSet(D3DRENDERSTATE_TEXTUREMIN, (void*)renderStates[D3DRENDERSTATE_TEXTUREMIN]);
    RenderStateSet(D3DRENDERSTATE_TEXTUREMAG, (void*)renderStates[D3DRENDERSTATE_TEXTUREMAG]);
}

// Render function.
void ImGui_ImplDX6_RenderDrawData(ImDrawData* draw_data)
{
    ImGuiIO& io = ImGui::GetIO();

    if (!GetD3DDevice())
        return;

    if (io.DisplaySize.x <= 0.0f || io.DisplaySize.y <= 0.0f)
        return;

    if (!vertexBuffer || vertexBufferSize < draw_data->TotalVtxCount)
    {
        if (vertexBuffer) {
            delete[] vertexBuffer;
            vertexBuffer = NULL;
        }
        vertexBufferSize = draw_data->TotalVtxCount + 5000;
        vertexBuffer = new tVertex[vertexBufferSize];
    }

    float z = 1.0f / 1024.0f;
    float offset = -0.5f;

    tVertex* vtx_dst = vertexBuffer;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawVert* vtx_src = cmd_list->VtxBuffer.Data;
        for (int i = 0; i < cmd_list->VtxBuffer.Size; i++)
        {
            vtx_dst[i].x =  vtx_src[i].pos.x + offset;
            vtx_dst[i].y =  vtx_src[i].pos.y + offset;
            vtx_dst[i].z = z;
            vtx_dst[i].w = z;
            vtx_dst[i].diff = COLOR_RGBA(vtx_src[i].col & 0xFF, vtx_src[i].col >> 8 & 0xFF, vtx_src[i].col >> 16 & 0xFF, vtx_src[i].col >> 24 & 0xFF);
            vtx_dst[i].spec = 0;
            vtx_dst[i].u = vtx_src[i].uv.x;
            vtx_dst[i].v = vtx_src[i].uv.y;
        }
        vtx_dst += cmd_list->VtxBuffer.Size;
    }

    ImGui_ImplDX6_SetupRenderState();

    int vtx_offset = 0;
    ImVec2 clip_off = draw_data->DisplayPos;

    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        int idx_offset = 0;

        for (int i = 0; i < cmd_list->CmdBuffer.Size; i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[i];
            if (pcmd->UserCallback)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    ImGui_ImplDX6_SetupRenderState();
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec2 clip_min(pcmd->ClipRect.x - clip_off.x, pcmd->ClipRect.y - clip_off.y);
                ImVec2 clip_max(pcmd->ClipRect.z - clip_off.x, pcmd->ClipRect.w - clip_off.y);
                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                    continue;

                GetD3DDevice()->SetTexture(0, fontTexture);
                GetD3DDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
                    D3DFVF_TLVERTEX,
                    vertexBuffer + vtx_offset, cmd_list->VtxBuffer.Size,
                    cmd_list->IdxBuffer.Data + idx_offset, pcmd->ElemCount,
                    0);
            }
            idx_offset += pcmd->ElemCount;
        }
        vtx_offset += cmd_list->VtxBuffer.Size;
    }

    ImGui_ImplDX6_RestoreRenderState();
}

bool ImGui_ImplDX6_Init()
{
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplDX6_CreateDeviceObjects();

    init = true;

    return true;
}

void ImGui_ImplDX6_Shutdown()
{
    if (!init)
        return;

    ImGui_ImplDX6_InvalidateDeviceObjects();
    ImGui::DestroyContext();
    init = false;
}

static bool ImGui_ImplDX6_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height, bytes_per_pixel;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);

    // Upload texture to graphics system
    if (D3DTextr_CreateTexture(GetD3DDevice(), "imgui", width, height, 32, pixels, fontTexture) < 0) {
        return false;
    }

    // Store our identifier
    io.Fonts->SetTexID((ImTextureID)fontTexture);
    return true;
}

bool ImGui_ImplDX6_CreateDeviceObjects()
{
    if (!GetD3DDevice())
        return false;

    if (!ImGui_ImplDX6_CreateFontsTexture())
        return false;

    return true;
}

void ImGui_ImplDX6_InvalidateDeviceObjects()
{
    if (!GetD3DDevice())
        return;

    if (vertexBuffer) {
        delete[] vertexBuffer;
        vertexBuffer = NULL; 
    }

    if (fontTexture) { 
        D3DTextr_DestroyTexture("imgui");
        fontTexture = NULL;
        ImGui::GetIO().Fonts->SetTexID(NULL); 
    } 
}

void ImGui_ImplDX6_Reset() {
    if (!init)
        return;

    D3DTextr_Restore("imgui", GetD3DDevice(), fontTexture);
}

void ImGui_ImplDx6_NewFrame() {
    ImGuiIO& io = ImGui::GetIO();

    POINT p{};
    GetCursorPos(&p);
    ScreenToClient(GetHWnd(), &p);

    io.MousePos = ImVec2(p.x, p.y);

    io.MouseDown[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    io.MouseDown[1] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
}
