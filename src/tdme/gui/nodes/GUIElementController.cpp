#include <tdme/gui/nodes/GUIElementController.h>

#include <string>

#include <tdme/tdme.h>
#include <tdme/application/Application.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIKeyboardEvent.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeConditions.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/GUI.h>
#include <tdme/math/Math.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Time.h>

using std::string;
using std::to_string;

using tdme::application::Application;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIElementController;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::GUI;
using tdme::math::Math;
using tdme::utilities::Console;
using tdme::utilities::Time;

GUIElementController::GUIElementController(GUINode* node)
	: GUINodeController(node)
{
	this->isActionPerforming = false;
	this->disabled = required_dynamic_cast<GUIElementNode*>(node)->isDisabled();
	this->initialized = false;
}

string GUIElementController::CONDITION_DISABLED = "disabled";
string GUIElementController::CONDITION_ENABLED = "enabled";

bool GUIElementController::isDisabled()
{
	return disabled;
}

void GUIElementController::setDisabled(bool disabled)
{
	auto& nodeConditions = required_dynamic_cast<GUIElementNode*>(node)->getActiveConditions();
	nodeConditions.remove(this->disabled == true?CONDITION_DISABLED:CONDITION_ENABLED);
	this->disabled = disabled;
	nodeConditions.add(this->disabled == true?CONDITION_DISABLED:CONDITION_ENABLED);
	isActionPerforming = false;
}

void GUIElementController::initialize()
{
	this->node->getScreenNode()->addTickNode(this->node);
	setDisabled(disabled);
}

void GUIElementController::dispose()
{
}

void GUIElementController::postLayout()
{
}

void GUIElementController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
	if (disabled == true) return;
	auto elementNode = required_dynamic_cast<GUIElementNode*>(this->node);
	if (node == elementNode && elementNode->isEventBelongingToNode(event) == true && event->getButton() == MOUSE_BUTTON_LEFT) {
		event->setProcessed(true);
		if (event->getType() == GUIMouseEvent::MOUSEEVENT_PRESSED) {
			elementNode->getActiveConditions().add(GUIElementNode::CONDITION_CLICK);
			isActionPerforming = true;
			if (elementNode->isFocusable() == true) {
				node->getScreenNode()->getGUI()->setFoccussedNode(elementNode);
			}
			mouseLastX = event->getXUnscaled();
			mouseLastY = event->getYUnscaled();
		} else
		if (event->getType() == GUIMouseEvent::MOUSEEVENT_DRAGGED) {
			isActionPerforming = true;
			//
			if (Math::abs(mouseLastX - event->getX()) > 5 ||
				Math::abs(mouseLastY - event->getY()) > 5) {
				node->getScreenNode()->forwardDragRequest(elementNode, event->getXUnscaled(), event->getYUnscaled());
			}
		} else
		if (event->getType() == GUIMouseEvent::MOUSEEVENT_RELEASED) {
			elementNode->getActiveConditions().remove(GUIElementNode::CONDITION_CLICK);
			isActionPerforming = false;
			auto now = Time::getCurrentMillis();
			if (timeLastClicked != -1LL) {
				if (now - timeLastClicked < TIME_DOUBLECLICK) {
					auto onMouseDoubleClickExpression = elementNode->getOnMouseDoubleClickExpression();
					if (onMouseDoubleClickExpression.size() > 0) {
						GUIElementNode::executeExpression(elementNode->getScreenNode(), onMouseDoubleClickExpression);
					} else {
						auto onMouseClickExpression = elementNode->getOnMouseClickExpression();
						if (onMouseClickExpression.size() > 0) GUIElementNode::executeExpression(elementNode->getScreenNode(), onMouseClickExpression);
						node->getScreenNode()->forwardAction(GUIActionListenerType::PERFORMED, elementNode);
					}
					timeLastClicked = -1LL;
				} else {
					auto onMouseClickExpression = elementNode->getOnMouseClickExpression();
					if (onMouseClickExpression.size() > 0) GUIElementNode::executeExpression(elementNode->getScreenNode(), onMouseClickExpression);
					node->getScreenNode()->forwardAction(GUIActionListenerType::PERFORMED, elementNode);
					timeLastClicked = -1LL;
				}
			} else {
				auto onMouseDoubleClickExpression = elementNode->getOnMouseDoubleClickExpression();
				if (onMouseDoubleClickExpression.size() > 0) {
					timeLastClicked = now;
				} else {
					auto onMouseClickExpression = elementNode->getOnMouseClickExpression();
					if (onMouseClickExpression.size() > 0) GUIElementNode::executeExpression(elementNode->getScreenNode(), onMouseClickExpression);
					node->getScreenNode()->forwardAction(GUIActionListenerType::PERFORMED, elementNode);
				}
			}
			//
			mouseLastX = -1;
			mouseLastY = -1;
		}
	} else
	if (node == elementNode && elementNode->isEventBelongingToNode(event) == true && event->getButton() == MOUSE_BUTTON_RIGHT) {
		if (event->getType() == GUIMouseEvent::MOUSEEVENT_PRESSED) {
			node->getScreenNode()->forwardContextMenuRequest(elementNode, event->getXUnscaled(), event->getYUnscaled());
		}
		event->setProcessed(true);
	} else
	if (node == elementNode && event->getType() == GUIMouseEvent::MOUSEEVENT_RELEASED && event->getButton() == MOUSE_BUTTON_LEFT && elementNode->getActiveConditions().has(GUIElementNode::CONDITION_CLICK)) {
		elementNode->getActiveConditions().remove(GUIElementNode::CONDITION_CLICK);
		event->setProcessed(true);
	} else
	if (event->getType() == GUIMouseEvent::MOUSEEVENT_MOVED) {
		if (elementNode->isEventBelongingToNode(event) == true) {
			if (elementNode->getOptionValue("mouse-cursor") == "hand") Application::setMouseCursor(MOUSE_CURSOR_HAND);
			node->getScreenNode()->forwardMouseOver(elementNode);
			elementNode->getActiveConditions().add(GUIElementNode::CONDITION_ONMOUSEOVER);
			node->getScreenNode()->getGUI()->addMouseOutCandidateNode(elementNode);
			auto onMouseOverExpression = elementNode->getOnMouseOverExpression();
			if (onMouseOverExpression.size() > 0) GUIElementNode::executeExpression(elementNode->getScreenNode(), onMouseOverExpression);
		} else {
			if (elementNode->getOptionValue("mouse-cursor") == "hand") Application::setMouseCursor(MOUSE_CURSOR_NORMAL);
			elementNode->getActiveConditions().remove(GUIElementNode::CONDITION_ONMOUSEOVER);
			auto onMouseOutExpression = elementNode->getOnMouseOutExpression();
			if (onMouseOutExpression.size() > 0) GUIElementNode::executeExpression(elementNode->getScreenNode(), onMouseOutExpression);
		}
	} else {
		isActionPerforming = false;
	}
}

void GUIElementController::handleKeyboardEvent(GUIKeyboardEvent* event)
{
	if (disabled == false) {
		switch (event->getKeyCode()) {
			case GUIKeyboardEvent::KEYCODE_SPACE: {
					event->setProcessed(true);
					if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) {
						auto onMouseClickExpression = required_dynamic_cast<GUIElementNode*>(node)->getOnMouseClickExpression();
						if (onMouseClickExpression.size() > 0) GUIElementNode::executeExpression(node->getScreenNode(), onMouseClickExpression);
						node->getScreenNode()->forwardAction(GUIActionListenerType::PERFORMED, required_dynamic_cast<GUIElementNode*>(node));
					}
				}
				break;
			default: {
					break;
				}
		}
	}
}

void GUIElementController::tick()
{
	if (initialized == false) {
		auto onInitializeExpression = required_dynamic_cast<GUIElementNode*>(node)->getOnInitializeExpression();
		if (onInitializeExpression.size() > 0) GUIElementNode::executeExpression(node->getScreenNode(), onInitializeExpression);
		initialized = true;
		// TODO: check me, the following code has performance relevance!!!
		// auto onMouseClickExpression = required_dynamic_cast<GUIElementNode*>(node)->getOnMouseClickExpression();
		// if (onMouseClickExpression.size() == 0) node->getScreenNode()->removeTickNode(node);
	}

	auto now = Time::getCurrentMillis();
	if (timeLastClicked != -1LL && now - timeLastClicked >= TIME_DOUBLECLICK) {
		timeLastClicked = -1LL;
		auto onMouseClickExpression = required_dynamic_cast<GUIElementNode*>(node)->getOnMouseClickExpression();
		if (onMouseClickExpression.size() > 0) GUIElementNode::executeExpression(node->getScreenNode(), onMouseClickExpression);
		node->getScreenNode()->forwardAction(GUIActionListenerType::PERFORMED, required_dynamic_cast<GUIElementNode*>(node));
	}

	if (isActionPerforming == true) {
		if (disabled == true) {
			isActionPerforming = false;
			return;
		}
		node->getScreenNode()->forwardAction(GUIActionListenerType::PERFORMING, required_dynamic_cast<GUIElementNode*>(node));
	}
}

void GUIElementController::onFocusGained()
{
}

void GUIElementController::onFocusLost()
{
}

bool GUIElementController::hasValue()
{
	return false;
}

const MutableString& GUIElementController::getValue()
{
	return value;
}

void GUIElementController::setValue(const MutableString& value)
{
}

void GUIElementController::onSubTreeChange()
{
}
