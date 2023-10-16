#include "gui/GridComponent.h"
#include "gui/GraphicsTimer.h"
#include "gui/EasingAnimator.h"

class BlockGridComponent: public GridComponent, public GraphicsTimer {
public:
  BlockGridComponent(Config config);
  void highlightColumn(int start, int end);
  void update(const float secondsSinceLastUpdate) override;
  void gridItemStretchEnded(GridItemComponent* item, int offset) override;
  void resetDots();
  void snapItem(GridItemComponent* item, Index index, bool resetBounds = false) override;
  virtual void setItemLength(GridItemComponent* moduleComponent, int length) override;
  void reset();
  void setDotsHidden(bool hidden);
  void applyMouseHoverEffect();
  void animateDragMode(GridItemComponent* item, bool enabled = true);
  OwnedArray<OwnedArray<DotComponent>> dots;
  void clear() override;
protected:
  void itemHovered(GridItemComponent* item, bool valid, bool inside, int proposedLength, Index index) override;
  virtual void itemLandedOutside(GridItemComponent* item, Index index) override;
  virtual void gridItemEndedDrag(GridItemComponent* item, const MouseEvent& mouseEvent) override;
  virtual void gridItemStartedDrag(GridItemComponent* item, const MouseEvent& mouseEvent) override;
private:
  bool highlight = false;
  Rectangle<int> getBoundsForPlaceholder(Index& index, int length);
  inline bool isIndexInside(Index index) { return !isIndexOutside(index); }
  BlockPlaceholder blockPlaceholder;
  EasingAnimator animator;
  void spawnDots();
  void resizeDots();
  void resized() override;
  void hideDotsAroundIndex(GridItemComponent* item, Index index, int length, bool visible);
  float sineX = 0.0f;
  float defaultAlpha = 0.25f;
  int firstHighlightedColumn, lastHighlightedColumn;
};
