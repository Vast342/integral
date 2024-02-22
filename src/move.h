#ifndef INTEGRAL_MOVE_H_
#define INTEGRAL_MOVE_H_

#include "types.h"

const U8 kMoveMask = 0b111111;
const U8 kPieceTypeMask = 0b111;
const U8 kPromotionTypeMask = 0b111;

// bits 0-5: from
// bits 6-11: to
// bits 12-14: piece type
// bits 15-17: promotion type
struct Move {
  U32 data = 0;

  Move(U8 from, U8 to) {
    data = (from & kMoveMask) + ((to & kMoveMask) << 6);
  }

  Move(U8 from, U8 to, PieceType piece_type) : Move(from, to) {
    data |= (static_cast<U8>(piece_type) & kPieceTypeMask) << 12;
  }

  Move(U8 from, U8 to, PieceType piece_type, PromotionType promotion_type) : Move(from, to, piece_type) {
    data |= (static_cast<U8>(promotion_type) & kPromotionTypeMask) << 15;
  }

  [[nodiscard]] U8 get_from() const {
    return data & kMoveMask;
  }

  [[nodiscard]] U8 get_to() const {
    return (data >> 6) & kMoveMask;
  }

  [[nodiscard]] PieceType get_piece_type() const {
    return PieceType((data >> 12) & kPieceTypeMask);
  }
};

#endif // INTEGRAL_MOVE_H_