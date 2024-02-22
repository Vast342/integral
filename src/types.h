#ifndef INTEGRAL_TYPES_H_
#define INTEGRAL_TYPES_H_

using U8 = unsigned char;
using U32 = unsigned int;
using U64 = unsigned long long;

enum class PieceType : U8 {
  kNone,
  kPawn,
  kKnight,
  kBishop,
  kRook,
  kQueen,
  kKing,
};

enum class PromotionType : U8 {
  kNone,
  kKnight,
  kBishop,
  kRook,
  kQueen
};

enum class Color {
  kWhite,
  kBlack,
  kNone
};

enum CastleRights : U8 {
  kWhiteKingside = 0b0001,
  kWhiteQueenside = 0b0010,
  kWhiteBothSides = kWhiteKingside | kWhiteQueenside,
  kBlackKingside = 0b0100,
  kBlackQueenside = 0b1000,
  kBlackBothSides = kBlackKingside | kBlackQueenside,
};

enum class Direction : U8 {
  kNorth,
  kSouth,
  kEast,
  kWest,
  kNorthEast,
  kNorthWest,
  kSouthEast,
  kSouthWest
};

#endif // INTEGRAL_TYPES_H_