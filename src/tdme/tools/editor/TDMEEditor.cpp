#include <tdme/tools/editor/TDMEEditor.h>

#include <cstdlib>
#include <string>

#include <tdme/tdme.h>
#include <tdme/utilities/Time.h>

#include <tdme/engine/model/Color4.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/Prototype_Type.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/SimplePartition.h>
#include <tdme/engine/Version.h>
#include <tdme/gui/GUI.h>
#include <tdme/tools/editor/misc/PopUps.h>
#include <tdme/tools/editor/misc/Tools.h>
#include <tdme/tools/editor/views/EditorView.h>
#include <tdme/tools/editor/views/View.h>
#include <tdme/utilities/Console.h>

using std::string;

using tdme::tools::editor::TDMEEditor;

using tdme::utilities::Time;

using tdme::engine::model::Color4;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::Prototype_Type;
using tdme::engine::Engine;
using tdme::engine::SimplePartition;
using tdme::engine::Version;
using tdme::gui::GUI;
using tdme::tools::editor::misc::PopUps;
using tdme::tools::editor::misc::Tools;
using tdme::tools::editor::views::EditorView;
using tdme::tools::editor::views::View;
using tdme::utilities::Console;

TDMEEditor* TDMEEditor::instance = nullptr;

TDMEEditor::TDMEEditor()
{
	Tools::loadSettings(this);
	TDMEEditor::instance = this;
	engine = Engine::getInstance();
	engine->setPartition(new SimplePartition());
	view = nullptr;
	viewInitialized = false;
	viewNew = nullptr;
	popUps = new PopUps();
	editorView = nullptr;
	quitRequested = false;
}

TDMEEditor::~TDMEEditor() {
	delete popUps;
	delete editorView;
}

void TDMEEditor::main(int argc, char** argv)
{
	Console::println(string("TDMEEditor ") + Version::getVersion());
	Console::println(Version::getCopyright());
	Console::println();

	auto tdmeEditor = new TDMEEditor();
	tdmeEditor->run(argc, argv, "TDMEEditor", nullptr, Application::WINDOW_HINT_MAXIMIZED);
}

void TDMEEditor::setView(View* view)
{
	viewNew = view;
}

View* TDMEEditor::getView()
{
	return view;
}

void TDMEEditor::quit()
{
	quitRequested = true;
}

void TDMEEditor::display()
{
	if (viewNew != nullptr) {
		if (view != nullptr && viewInitialized == true) {
			view->deactivate();
			view->dispose();
			viewInitialized = false;
		}
		view = viewNew;
		viewNew = nullptr;
	}
	if (view != nullptr) {
		if (viewInitialized == false) {
			view->initialize();
			view->activate();
			viewInitialized = true;
		}
		view->display();
	}
	engine->display();
	engine->getGUI()->handleEvents();
	engine->getGUI()->render();
	if (view != nullptr) view->tick();
	if (quitRequested == true) {
		if (view != nullptr) {
			view->deactivate();
			view->dispose();
		}
		Application::exit(0);
	}
}

void TDMEEditor::dispose()
{
	if (view != nullptr && viewInitialized == true) {
		view->deactivate();
		view->dispose();
		view = nullptr;
	}
	engine->dispose();
	popUps->dispose();
	Tools::oseDispose();
}

void TDMEEditor::initialize()
{
	engine->initialize();
	// TODO: settings maybe for the next 2
	Application::setVSyncEnabled(true);
	Application::setLimitFPS(true);
	engine->setSceneColor(Color4(125.0f / 255.0f, 125.0f / 255.0f, 125.0f / 255.0f, 1.0f));
	setInputEventHandler(engine->getGUI());
	Tools::oseInit();
	popUps->initialize();
	setView(editorView = new EditorView(popUps));
}

void TDMEEditor::reshape(int width, int height)
{
	engine->reshape(width, height);
}
