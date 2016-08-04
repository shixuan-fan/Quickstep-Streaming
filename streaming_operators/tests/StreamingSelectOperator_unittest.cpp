/**
 *   Created in the Quickstep Research Group, University of Wisconsin-Madison.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 **/

#include <algorithm>
#include <vector>

#include "basics/Common.hpp"
#include "catalog/CatalogAttribute.hpp"
#include "catalog/CatalogRelationSchema.hpp"
#include "expressions/predicate/ComparisonPredicate.hpp"
#include "expressions/predicate/Predicate.hpp"
#include "expressions/scalar/Scalar.hpp"
#include "expressions/scalar/ScalarAttribute.hpp"
#include "expressions/scalar/ScalarLiteral.hpp"
#include "streaming_operators/StreamingSelectOperator.hpp"
#include "types/CharType.hpp"
#include "types/TypeFactory.hpp"
#include "types/TypeID.hpp"
#include "types/TypedValue.hpp"
#include "types/operations/comparisons/ComparisonFactory.hpp"
#include "types/operations/comparisons/ComparisonID.hpp"
#include "types/containers/Tuple.hpp"
#include "types/containers/TupleVectorValueAccessor.hpp"


#include "gtest/gtest.h"

namespace quickstep {

namespace {

  constexpr std::size_t kQueryID = 2;
  constexpr int kNumTuples = 100;
  constexpr int kRelationID = 1;
  constexpr int kLowerBound = 30;
  constexpr int kUpperBound = 70;
  constexpr int kNumInputs = 3;
  constexpr int kIntIndex = 0;
  constexpr int kBaseIndex = 3;
  constexpr int kBaseIndexAfter = 1;

}  // namespace

// The parameter tells whether a predicate is used when selecting.
class StreamingSelectOperatorTest : public ::testing::TestWithParam<bool> {
 protected:
  virtual void SetUp() {
    // Set up operator.
    select_operator_.reset(new StreamingSelectOperator(kQueryID));

    // Set up CatalogRelationSchema.
    schema_.reset(new CatalogRelationSchema(nullptr, "test", kRelationID));

    // Add different kinds of attributes: int, double, char
    CatalogAttribute *int_attr = new CatalogAttribute(schema_.get(),
                                                      "int_attr",
                                                      TypeFactory::GetType(kInt, true));

    schema_->addAttribute(int_attr);

    CatalogAttribute *double_attr = new CatalogAttribute(schema_.get(),
                                                         "double_attr",
                                                         TypeFactory::GetType(kDouble, true));
    schema_->addAttribute(double_attr);

    CatalogAttribute *char_attr = new CatalogAttribute(schema_.get(),
                                                       "char_attr",
                                                       TypeFactory::GetType(kChar, 4, true));
    schema_->addAttribute(char_attr);

    // Base value will increase 1 at a time and will never be NULL.
    CatalogAttribute *base_value_attr = new CatalogAttribute(schema_.get(),
                                                             "base_value",
                                                             TypeFactory::GetType(kInt, false));
    schema_->addAttribute(base_value_attr);

    // Set up input accessor.
    inputs_.reset(new std::vector<TupleVectorValueAccessor*>());
    for (int i = 0; i < kNumInputs; i++) {
      TupleVectorValueAccessor *accessor = new TupleVectorValueAccessor();
      for (int j = 0; j < kNumTuples / (i + 1); j++) {
        accessor->addTuple(createTuple(j));
      }

      inputs_->push_back(accessor);
    }
  }

  // Create tuple upon base_value.
  Tuple createTuple(const int base_value) const {
    std::vector<TypedValue> attrs;

    // int_attr
    if (base_value % 8 == 0) {
      // Throw in a NULL integer for every eighth value.
      attrs.emplace_back(kInt);
    } else {
      attrs.emplace_back(base_value);
    }

    // double_attr
    if (base_value % 8 == 2) {
      // NULL very eighth value.
      attrs.emplace_back(kDouble);
    } else {
      attrs.emplace_back(static_cast<double>(0.25 * base_value));
    }

    // char_attr
    if (base_value % 8 == 4) {
      // NULL very eighth value.
      attrs.emplace_back(CharType::InstanceNullable(4).makeNullValue());
    } else {
      std::ostringstream char_buffer;
      char_buffer << base_value;
      std::string string_literal(char_buffer.str());
      attrs.emplace_back(CharType::InstanceNonNullable(4).makeValue(
          string_literal.c_str(),
          string_literal.size() > 3 ? 4
                                    : string_literal.size() + 1));
      attrs.back().ensureNotReference();
    }

    // base_value
    attrs.emplace_back(base_value);
    return Tuple(std::move(attrs));
  }

  // Reset predicates.
  void resetPredicates(int lower_bound_val, int upper_bound_val) {
    predicates_.reset(new std::vector<std::unique_ptr<const Predicate>>());
    Scalar *base_scalar_lower = new ScalarAttribute(*schema_->getAttributeByName("base_value"));
    Scalar *lower_bound =
        new ScalarLiteral(TypedValue(lower_bound_val),
                          TypeFactory::GetType(kInt, false));
    Predicate *lower_bound_predicate =
        new ComparisonPredicate(ComparisonFactory::GetComparison(ComparisonID::kLessOrEqual),
                                lower_bound,
                                base_scalar_lower);
    predicates_->emplace_back(lower_bound_predicate);

    Scalar *base_scalar_upper = new ScalarAttribute(*schema_->getAttributeByName("base_value"));
    Scalar *upper_bound =
        new ScalarLiteral(TypedValue(upper_bound_val),
                          TypeFactory::GetType(kInt, false));
    Predicate *upper_bound_predicate =
        new ComparisonPredicate(ComparisonFactory::GetComparison(ComparisonID::kLessOrEqual),
                                base_scalar_upper,
                                upper_bound);
    predicates_->emplace_back(upper_bound_predicate);
  }

  void checkOutput(const std::vector<TupleVectorValueAccessor*> &outputs) {
    for (int i = 0; i < kNumInputs; i++) {
      // Check each outputs' tuple count.
      int start = 0, end = kNumTuples / (i + 1);
      if (GetParam()) {
        start = std::max(kLowerBound, start);
        end = std::min(kUpperBound + 1, end);
      }

      EXPECT_EQ(end - start, outputs[i]->getNumTuples());

      // Check each tuples' selected attributes.
      TupleVectorValueAccessor *output = outputs[i];
      output->beginIteration();
      while (start < end) {
        output->next();
        EXPECT_EQ(start, output->getTypedValue(kBaseIndexAfter).getLiteral<int>());

        if (start % 8 == 0) {
          EXPECT_TRUE(output->getTypedValue(kIntIndex).isNull());
        } else {
          EXPECT_EQ(start, output->getTypedValue(kIntIndex).getLiteral<int>());
        }
        
        start++;
      }
    }
  }

  std::unique_ptr<StreamingSelectOperator> select_operator_;
  std::unique_ptr<std::vector<TupleVectorValueAccessor*>> inputs_;
  std::unique_ptr<CatalogRelationSchema> schema_;
  std::unique_ptr<std::vector<std::unique_ptr<const Predicate>>> predicates_;
};

TEST_F(StreamingSelectOperatorTest, OpenCloseTest) {
  std::vector<std::unique_ptr<const Predicate>> predicates;
  std::vector<attribute_id> select_attributes;
  QuerySpec *select_spec =
      new SelectSpec(std::move(predicates), std::move(select_attributes));
  // open() should only succeed when the operator is closed.
  EXPECT_TRUE(select_operator_->open(select_spec));
  EXPECT_FALSE(select_operator_->open(select_spec));

  // close() should only succeed when the operator is open.
  EXPECT_TRUE(select_operator_->close());
  EXPECT_FALSE(select_operator_->close());
}

TEST_P(StreamingSelectOperatorTest, SimpleProjectionTest) {
  // Predicates.
  if (GetParam()) {
    resetPredicates(kLowerBound, kUpperBound);
  } else {
    predicates_.reset(new std::vector<std::unique_ptr<const Predicate>>());
  }

  // Select attributes.
  std::vector<attribute_id> select_attributes;
  select_attributes.push_back(kIntIndex);
  select_attributes.push_back(kBaseIndex);

  // Select specs.
  QuerySpec *select_spec =
      new SelectSpec(std::move(*predicates_.release()), std::move(select_attributes));
  select_operator_->open(select_spec);

  // Make output vectors and compare.
  std::vector<TupleVectorValueAccessor*> outputs;
  select_operator_->next(*inputs_, &outputs);
  checkOutput(outputs);
  select_operator_->close();
}

TEST_P(StreamingSelectOperatorTest, ArbitraryExpressionTest) {
  // Predicates.
  if (GetParam()) {
    resetPredicates(kLowerBound, kUpperBound);
  } else {
    predicates_.reset(new std::vector<std::unique_ptr<const Predicate>>());
  }

  // Select expressions.
  std::vector<std::unique_ptr<const Scalar>> select_expressions;
  select_expressions.emplace_back(
      new ScalarAttribute(*schema_->getAttributeById(kIntIndex)));
  select_expressions.emplace_back(
      new ScalarAttribute(*schema_->getAttributeById(kBaseIndex)));

  // Select specs.
  QuerySpec *select_spec =
      new SelectSpec(std::move(*predicates_.release()), std::move(select_expressions));
  select_operator_->open(select_spec);

  // Make output vectors and compare.
  std::vector<TupleVectorValueAccessor*> outputs;
  select_operator_->next(*inputs_, &outputs);
  checkOutput(outputs);
  select_operator_->close();
}

INSTANTIATE_TEST_CASE_P(WithOrWithoutPredicate, StreamingSelectOperatorTest, ::testing::Bool());

}  // namespace quickstep
