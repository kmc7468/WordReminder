#include "LayoutEngine.hpp"

#include <cassert>
#include <utility>

Length::Length(std::function<double(const LengthEvaluationContext&)> evaluator) noexcept
	: m_Evaluator(std::move(evaluator)) {
	assert(m_Evaluator != nullptr);
}

void Length::Evaluate(const LengthEvaluationContext& evaluationContext) {
	m_Evaluated = m_Evaluator(evaluationContext);

	assert(m_Evaluated != 0);
}
double Length::GetEvaluated() const noexcept {
	assert(m_Evaluated != 0);

	return m_Evaluated;
}

Length Length::Pixel(double value, ScaleMode scaleMode) {
	assert(value > 0);
	assert(scaleMode == DisplayBased || scaleMode == WindowBased);

	return Length([value, scaleMode](const LengthEvaluationContext& evaluationContext) {
		return value * (scaleMode == DisplayBased ? evaluationContext.DisplayScale : evaluationContext.WindowScale);
	});
}
Length Length::Percent(double value) {
	assert(value > 0);
	assert(value <= 100);

	return Length([value](const LengthEvaluationContext& evaluationContext) {
		return evaluationContext.ParentLength * value / 100;
	});
}
Length Length::WrapContent() {
	return Length([](const LengthEvaluationContext&) {
		return -1;
	});
}

Division::Division(std::wstring name, Length width, Length height) noexcept
	: m_Name(std::move(name)), m_Width(std::move(width)), m_Height(std::move(height)) {
	assert(!m_Name.empty());
}
Division::Division(Component* component, Length width, Length height) noexcept
	: m_Component(component), m_Width(std::move(width)), m_Height(std::move(height)) {
	assert(component != nullptr);
}
Division::Division(std::wstring name, Component* component, Length width, Length height) noexcept
	: m_Name(std::move(name)), m_Component(component), m_Width(std::move(width)), m_Height(std::move(height)) {
	assert(!m_Name.empty());
	assert(component != nullptr);
}

void Division::AddChild(Division&& child) {
	m_Children.push_back(std::move(child));
}

Division& Division::SetMargin(const Length& margin) {
	m_LeftMargin = m_RightMargin = m_TopMargin = m_BottomMargin = margin;
	return *this;
}
Division& Division::SetHorizontalMargin(const Length& horizontalMargin) {
	m_LeftMargin = m_RightMargin = horizontalMargin;
	return *this;
}
Division& Division::SetVerticalMargin(const Length& verticalMargin) {
	m_TopMargin = m_BottomMargin = verticalMargin;
	return *this;
}
Division& Division::SetLeftMargin(Length leftMargin) noexcept {
	m_LeftMargin = std::move(leftMargin);
	return *this;
}
Division& Division::SetRightMargin(Length rightMargin) noexcept {
	m_RightMargin = std::move(rightMargin);
	return *this;
}
Division& Division::SetTopMargin(Length topMargin) noexcept {
	m_TopMargin = std::move(topMargin);
	return *this;
}
Division& Division::SetBottomMargin(Length bottomMargin) noexcept {
	m_BottomMargin = std::move(bottomMargin);
	return *this;
}
Division& Division::SetPadding(const Length& padding) {
	m_LeftPadding = m_RightPadding = m_TopPadding = m_BottomPadding = padding;
	return *this;
}
Division& Division::SetHorizontalPadding(const Length& horizontalPadding) {
	m_LeftPadding = m_RightPadding = horizontalPadding;
	return *this;
}
Division& Division::SetVerticalPadding(const Length& verticalPadding) {
	m_TopPadding = m_BottomPadding = verticalPadding;
	return *this;
}
Division& Division::SetLeftPadding(Length leftPadding) noexcept {
	m_LeftPadding = std::move(leftPadding);
	return *this;
}
Division& Division::SetRightPadding(Length rightPadding) noexcept {
	m_RightPadding = std::move(rightPadding);
	return *this;
}
Division& Division::SetTopPadding(Length topPadding) noexcept {
	m_TopPadding = std::move(topPadding);
	return *this;
}
Division& Division::SetBottomPadding(Length bottomPadding) noexcept {
	m_BottomPadding = std::move(bottomPadding);
	return *this;
}

Division& Division::SetHorizontalAlignment(Alignment horizontalAlignment) noexcept {
	m_HorizontalAlignment = horizontalAlignment;
	return *this;
}
Division& Division::SetVerticalAlignment(Alignment verticalAlignment) noexcept {
	m_VerticalAlignment = verticalAlignment;
	return *this;
}

namespace {
	LengthEvaluationContext& UpdateParentLength(LengthEvaluationContext& evaluationContext, double newParentLength) noexcept {
		evaluationContext.ParentLength = newParentLength;
		return evaluationContext;
	}
	LengthEvaluationContext& UpdateUsableLength(LengthEvaluationContext& evaluationContext, double newUsableLength) noexcept {
		evaluationContext.UsableLength = newUsableLength;
		return evaluationContext;
	}

#define UPL(newParentLength) UpdateParentLength(evaluationContext, newParentLength)
#define UUL(newUsableLength) UpdateUsableLength(evaluationContext, newUsableLength)
#define UPUL(newParentLength, newUsableLength) UPL(UUL(newUsableLength), newParentLength)
}

LayoutEngine::LayoutEngine(Component* component)
	: m_RootDivision(component, Length::Percent(100), Length::Percent(100)) {}

void LayoutEngine::Evaluate() {
	Component* windowComponent = m_RootDivision.m_Component;
	while (windowComponent->GetParent()) {
		windowComponent = windowComponent->GetParent();
	}

	Window* const window = dynamic_cast<Window*>(windowComponent);
	const Size windowSize = window->GetSize();

	LengthEvaluationContext evaluationContext;
	evaluationContext.DisplayScale = window->GetDisplayScale();
	evaluationContext.WindowScale =
		(windowSize.Width * 3 >= windowSize.Height * 4 ? windowSize.Height : windowSize.Width * 3. / 4) / 480;

	m_RootDivision.m_Width.Evaluate(UPL(windowSize.Width));
	m_RootDivision.m_Height.Evaluate(UPL(windowSize.Height));

	for (auto& child : m_RootDivision.m_Children) {
		Evaluate(child, m_RootDivision, evaluationContext);
	}
}

void LayoutEngine::Evaluate(Division& division, Division& parent, LengthEvaluationContext& evaluationContext) {
	// TODO
}