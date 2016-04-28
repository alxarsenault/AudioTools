/*
 * Copyright (c) 2016 AudioTools - All Rights Reserved
 *
 * This Software may not be distributed in parts or its entirety
 * without prior written agreement by AudioTools.
 *
 * Neither the name of the AudioTools nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY AUDIOTOOLS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AUDIOTOOLS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Written by Alexandre Arsenault <alx.arsenault@gmail.com>
 */
 
#include "PythonWrapper.h"
#include "atEditor.h"
#include "atEditorMainWindow.h"
#include "PanelPyWrapper.h"
#include "ButtonPyWrapper.h"
#include "WindowPyWrapper.h"

BOOST_PYTHON_MODULE(ax)
{
	// Create ax::Window python wrapper.
	ax::python::export_python_wrapper_window();
	
	ax::python::export_python_wrapper_utils();
	
	// Create ax::Panel python wrapper.
	ax::python::export_python_wrapper_panel();
	
	// Create ax::Button python wrapper.
	ax::python::export_python_wrapper_button();

	boost::python::class_<ax::python::Widgets>("Widgets").def("Get", &ax::python::Widgets::Get);
}

namespace ax {
namespace python {
	std::shared_ptr<ax::python::Widgets> Widgets::instance = nullptr;
	
	std::shared_ptr<ax::python::Widgets> Widgets::GetInstance()
	{
		if(instance == nullptr) {
			instance = std::shared_ptr<ax::python::Widgets>(new Widgets());
		}
		
		return instance;
	}
	
	Widgets::Widgets()
	{
		_pt = boost::shared_ptr<ax::Point>(new ax::Point(14, 642));
	}

	boost::python::object Widgets::Get(const std::string& widget_name)
	{
		ax::Window* win = at::editor::App::GetInstance()->GetMainWindow()->GetWidgetsByName(widget_name);
	
		if(win == nullptr) {
			return boost::python::object();
		}
		
		widget::Component* widget = static_cast<widget::Component*>(win->component.Get("Widget").get());
		
		const std::string builder_name(widget->GetBuilderName());
		
		if(builder_name == "Panel") {
			ax::Panel* panel = static_cast<ax::Panel*>(win->backbone.get());
			return boost::python::object(ax::python::Panel(panel));
		}
		else if(builder_name == "Button") {
			ax::Button* btn = static_cast<ax::Button*>(win->backbone.get());
			return boost::python::object(ax::python::Button(btn));
		}
		
	
		return boost::python::object(boost::python::ptr(_pt.get()));
	}
	
	void InitWrapper()
	{
		initax();
	}
}
}