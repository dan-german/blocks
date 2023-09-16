/*
  ==============================================================================

    Index.h
    Created: 20 Dec 2021 1:23:01pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once

struct Index {
  int row, column;
  Index(int row, int column): row(row), column(column) { }
  inline Index toTheRight(int times) { return Index { row, column + times }; }
  inline bool operator==(const Index& other) const { return other.row == row && other.column == column; }
  inline bool operator!=(const Index& other) const { return !(other.row == row && other.column == column); }
};