//#include "mdiControl.hpp"
//#include "mdiCtrlButton.hpp"
//#include "mdiCtrlKnob.hpp"

#include "atEditorGridWindow.h"
#include <OpenAX/WindowManager.h>
#include <OpenAX/rapidxml.hpp>
#include <OpenAX/rapidxml_print.hpp>
#include <fstream>

#include "atCommon.h"
#include "PyoAudio.h"
#include "PyoComponent.h"
#include "atEditorLoader.h"

#include <OpenAX/WidgetLoader.h>
#include <OpenAX/Button.h>
#include <OpenAX/Knob.h>
#include <OpenAX/Label.h>
#include <OpenAX/Panel.h>
#include <OpenAX/Toggle.h>
#include <OpenAX/Slider.h>

namespace at {
namespace editor {
	GridWindow::GridWindow(const ax::Rect& rect)
		: _grid_space(10)
		, _selection(false, ax::Rect(0, 0, 0, 0))
		, _bg_color(1.0)
	{
		// Create window.
		win = ax::Window::Create(rect);
		win->event.OnPaint = ax::WBind<ax::GC>(this, &GridWindow::OnPaint);
		win->event.OnMouseLeftDown
			= ax::WBind<ax::Point>(this, &GridWindow::OnMouseLeftDown);
		win->event.OnMouseLeftDragging
			= ax::WBind<ax::Point>(this, &GridWindow::OnMouseLeftDragging);
		win->event.OnMouseLeftUp
			= ax::WBind<ax::Point>(this, &GridWindow::OnMouseLeftUp);

		win->property.AddProperty("BlockDrawing");
		win->property.AddProperty("AcceptWidget");

		ax::widget::Loader* loader = ax::widget::Loader::GetInstance();
		loader->AddBuilder("Button", new ax::Button::Builder());
		loader->AddBuilder("Toggle", new ax::Toggle::Builder());
		loader->AddBuilder("Knob", new ax::Knob::Builder());
		loader->AddBuilder("Label", new ax::Label::Builder());
		loader->AddBuilder("Panel", new ax::Panel::Builder());
		loader->AddBuilder("Slider", new ax::Slider::Builder());
		
		
		OpenLayout("layouts/default.xml");
	}

	void GridWindow::SetGridSpace(const int& space)
	{
		if (space > 0 && _grid_space != space) {
			_grid_space = space;
			win->Update();
		}
	}
	
	ax::Window* GridWindow::GetMainWindow()
	{
		std::vector<ax::Window::Ptr>& children = win->node.GetChildren();
		for(auto& n : children) {
			if(n->property.HasProperty("MainWindow")) {
				return n.get();
			}
		}
		
		return nullptr;
	}

	void GridWindow::SaveLayout(const std::string& path, const std::string& script_path)
	{
		std::vector<ax::Window::Ptr>& children = win->node.GetChildren();

		ax::Xml xml;
		ax::Xml::Node layout = xml.CreateNode("Layout");
		xml.AddMainNode(layout);
		layout.AddAttribute("script", script_path);

		ax::Print("Childen size :", children.size());

		// Callback for saving widget with child widgets in them.
		std::function<void(ax::Xml&, ax::Xml::Node&, ax::Window*)>
			panel_save_child = [&](
				ax::Xml& xml, ax::Xml::Node& node, ax::Window* child_win) {
				
				ax::widget::Component::Ptr opt
					= child_win->component.Get<ax::widget::Component>("Widget");

				if (opt) {
					if(child_win->property.HasProperty("AcceptWidget")) {
						opt->SetSaveChildCallback(panel_save_child);
					}
					// Save ax::Object.
					ax::Xml::Node child_node = opt->Save(xml, node);

					if (child_win->component.Has("pyo")) {
						pyo::Component::Ptr comp
							= child_win->component.Get<pyo::Component>("pyo");
						
						std::string fct_name = comp->GetFunctionName();
						
						ax::Xml::Node pyo_node = xml.CreateNode("pyo", fct_name);
						child_node.AddNode(pyo_node);
					}
				}
			};

		for (auto& n : children) {
			ax::widget::Component::Ptr opt
				= n->component.Get<ax::widget::Component>("Widget");

			if (opt) {
				if(n->property.HasProperty("AcceptWidget")) {
					opt->SetSaveChildCallback(panel_save_child);
				}
			
				// Save ax::Object.
				ax::Xml::Node node = opt->Save(xml, layout);

				if (n->component.Has("pyo")) {
					ax::Print("HAS PYO");
					pyo::Component::Ptr comp
						= n->component.Get<pyo::Component>("pyo");
					std::string fct_name = comp->GetFunctionName();
					ax::Xml::Node pyo_node = xml.CreateNode("pyo", fct_name);
					node.AddNode(pyo_node);
				}
			}
		}

		xml.Save(path);
	}

	std::string GridWindow::OpenLayout(const std::string& path)
	{
		at::editor::Loader loader(win);
		return loader.OpenLayout(path, true);
	}

	void GridWindow::SetBackgroundColor(const ax::Color& color)
	{
		_bg_color = color;
		win->Update();
	}

	void GridWindow::SetupControl(ax::Window* window)
	{
		auto m_down_fct = window->event.OnMouseLeftDown.GetFunction();
		auto m_drag_fct = window->event.OnMouseLeftDragging.GetFunction();
		auto m_up_fct = window->event.OnMouseLeftUp.GetFunction();
		auto m_right_down = window->event.OnMouseRightDown.GetFunction();

		ax::Window* gwin = win;

		window->event.OnMouseLeftDown = ax::WFunc<ax::Point>(
			[gwin, window, m_down_fct](const ax::Point& pos) {
				bool cmd_down
					= ax::App::GetInstance().GetWindowManager()->IsCmdDown();

				if (cmd_down) {
					ax::Point c_delta(
						pos - window->dimension.GetAbsoluteRect().position);
					window->resource.Add("click_delta", c_delta);
					window->event.GrabMouse();
					window->property.AddProperty("edit_click");

					ax::Print("LEFT DOWN CMD");
					gwin->PushEvent(
						1234, new ax::Event::SimpleMsg<ax::Window*>(window));
				}
				else {
					ax::Print("Mouse left down test");
					if (m_down_fct) {
						m_down_fct(pos);
					}
				}
			});

		window->event.OnMouseLeftDragging
			= ax::WFunc<ax::Point>([window, m_drag_fct](const ax::Point& pos) {
				  if (window->event.IsGrabbed()) {
					  //					win->resource.
					  if (window->property.HasProperty("edit_click")) {

						  ax::Point c_delta
							  = window->resource.GetResource("click_delta");
						  window->dimension.SetPosition(pos
							  - window->node.GetParent()
									->dimension.GetAbsoluteRect()
									.position
							  - c_delta);
					  }
				  }

				  if (m_drag_fct) {
					  m_drag_fct(pos);
				  }
			  });

		window->event.OnMouseLeftUp
			= ax::WFunc<ax::Point>([window, m_up_fct](const ax::Point& pos) {

				  window->property.RemoveProperty("edit_click");

				  if (m_up_fct) {
					  m_up_fct(pos);
				  }

				  if (window->event.IsGrabbed()) {
					  window->event.UnGrabMouse();
				  }
			  });

		window->event.OnMouseRightDown = ax::WFunc<ax::Point>(
			[window, m_right_down](const ax::Point& pos) {
				ax::Print("RIGHT DOWN");
				//				if (win->event.IsGrabbed()) {
				//					win->event.UnGrabMouse();
				//				}
				//
				window->Hide();
				window->DeleteWindow();

				if (m_right_down) {
					m_right_down(pos);
				}
			});
	}

	void GridWindow::OnMouseLeftDown(const ax::Point& pos)
	{
		bool cmd_down = ax::App::GetInstance().GetWindowManager()->IsCmdDown();

		// Clear menu.
		if (cmd_down) {
			UnSelectAllWidgets();
			win->PushEvent(UNSELECT_ALL, new ax::Event::SimpleMsg<int>(0));
		}
		// Start multi widget selection.
		else {
			ax::Point m_pos(pos - win->dimension.GetAbsoluteRect().position);
			_selection.first = true;
			_selection.second.position = m_pos;
			_selection.second.size = ax::Size(1, 1);

			win->event.GrabMouse();
			win->Update();
		}
	}

	void GridWindow::UnSelectAllWidgets()
	{
		std::vector<ax::Window::Ptr>& children = win->node.GetChildren();
		for (auto& n : children) {
			n->property.RemoveProperty("current_editing_widget");
			
			if(n->property.HasProperty("AcceptWidget")) {
				auto& c_children = n->node.GetChildren();
				
				/// @todo Do this recursively.
				for(auto& k : c_children) {
					k->property.RemoveProperty("current_editing_widget");
				}
			}
			
		}
		win->Update();
	}

	void GridWindow::OnMouseLeftDragging(const ax::Point& pos)
	{
		ax::Point m_pos(pos - win->dimension.GetAbsoluteRect().position);
		_selection.second.size = m_pos - _selection.second.position;
		win->Update();
	}

	void GridWindow::OnMouseLeftUp(const ax::Point& pos)
	{
		if (win->event.IsGrabbed()) {
			win->event.UnGrabMouse();
			_selection.first = false;
			win->Update();
		}
	}

	void GridWindow::OnPaint(ax::GC gc)
	{
		ax::Rect rect(win->dimension.GetDrawingRect());

		// Background.
		gc.SetColor(_bg_color);
		gc.DrawRectangle(rect);

		gc.SetColor(ax::Color(0.9));

		// Vertical lines.
		for (int x = _grid_space; x < rect.size.x; x += _grid_space) {
			gc.DrawLineStripple(ax::Point(x, 0), ax::Point(x, rect.size.y));
		}

		// Horizontal lines.
		for (int y = _grid_space; y < rect.size.y; y += _grid_space) {
			gc.DrawLineStripple(ax::Point(0, y), ax::Point(rect.size.x, y));
		}

		// Selection rectangle.
		if (_selection.first) {
			gc.SetColor(ax::Color(0.7, 0.5));
			gc.DrawRectangle(_selection.second);
			gc.SetColor(ax::Color(0.7, 0.8));
			gc.DrawRectangleContour(_selection.second);
		}

		// Grid contour.
		gc.SetColor(ax::Color(0.7));
		gc.DrawRectangleContour(rect);
	}
}
}