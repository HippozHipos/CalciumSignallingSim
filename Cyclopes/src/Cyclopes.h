#pragma once

#ifdef CYC_ENTRYPOINT_DEFINED
	#error "This file can only be included before Cyclopes/Core/EntryPoint.h"
#endif

#include "Cyclopes/Core/Window.h"
#include "Cyclopes/Core/Application.h"
#include "Cyclopes/Core/ResourceManager.h"

#include "Cyclopes/Core/Layer.h"
#include "Cyclopes/Core/LayerStack.h"

#include "Cyclopes/Core/Time.h"

#include "Cyclopes/Event/Keyboard.h"
#include "Cyclopes/Event/Mouse.h"
#include "Cyclopes/Event/WindowEvent.h"

#include "Cyclopes/Renderer/Renderer.h"
#include "Cyclopes/Renderer/Renderer2D.h"

#include "Cyclopes/Renderer/Camera.h"
#include "Cyclopes/Renderer/Camera2D.h"

#include "Cyclopes/Renderer/IndexBuffer.h"
#include "Cyclopes/Renderer/VertexBuffer.h"

#include "Cyclopes/Renderer/Shader.h"

#include "Cyclopes/Renderer/Graphics.h"
#include "Cyclopes/Renderer/GraphicsContext.h"
#include "Cyclopes/Renderer/Cycsh.h"

#include <imgui.h>
#include <implot.h>
