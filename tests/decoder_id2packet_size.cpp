#include <gtest/gtest.h>
#include <decup/decup.hpp>

TEST(decoder_id2data_size, small_data_size) {
  EXPECT_EQ(32uz, decup::decoder_id2data_size(200u));
  EXPECT_EQ(32uz, decup::decoder_id2data_size(202u));
  EXPECT_EQ(32uz, decup::decoder_id2data_size(203u));
  EXPECT_EQ(32uz, decup::decoder_id2data_size(204u));
  EXPECT_EQ(32uz, decup::decoder_id2data_size(205u));
}

TEST(decoder_id2data_size, large_data_size) {
  EXPECT_EQ(64uz, decup::decoder_id2data_size(1u));
  EXPECT_EQ(64uz, decup::decoder_id2data_size(201u));
  EXPECT_EQ(64uz, decup::decoder_id2data_size(255u));
}
