#include "gui/OutlinedLabel.h"
#include "settings/Constants.h"

void OutlinedLabel::paint(Graphics& g) {
  g.setColour(Colours::white.withAlpha(0.4f));
  g.fillRoundedRectangle(0, 0, getWidth(), getHeight() - trianglEdge, 3);
  Path path;

  Point<float> leftTrianglePoint = Point<float>(getWidth() / 2 - trianglEdge / 2, getHeight() - trianglEdge);
  Point<float> middleTrianglePoint = Point<float>(getWidth() / 2, getHeight() - trianglEdge * 0.6);
  Point<float> rightTrianglePoint = Point<float>(getWidth() / 2 + trianglEdge / 2, getHeight() - trianglEdge);

  path.startNewSubPath(leftTrianglePoint);
  path.lineTo(middleTrianglePoint);
  path.lineTo(rightTrianglePoint);

  g.fillPath(path);
}