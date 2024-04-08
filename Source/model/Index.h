/*
  ==============================================================================

    Index.h
    Created: 20 Dec 2021 1:23:01pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once

struct Index {
  int column, row;
  Index(int column, int row): column(column), row(row) { }
  ~Index() { }
  inline Index toTheRight(int times) { return Index { column, row + times }; }
  inline bool operator==(const Index& other) const { return other.column == column && other.row == row; }
  inline bool operator!=(const Index& other) const { return !(other.column == column && other.row == row); }
  inline Index operator+(const Index& other) const { return Index { column + other.column, row + other.row }; }
  inline Index operator-(const Index& other) const { return Index { column - other.column, row - other.row }; }
};