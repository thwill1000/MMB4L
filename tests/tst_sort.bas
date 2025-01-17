' Copyright (c) 2020-2024 Thomas Hugo Williams
' License MIT <https://opensource.org/licenses/MIT>
' For MMBasic 6.0

Option Explicit On
Option Default None
Option Base InStr(Mm.CmdLine$, "--base=1") > 0

#Include "../sptools/src/splib/system.inc"
#Include "../sptools/src/splib/array.inc"
#Include "../sptools/src/splib/list.inc"
#Include "../sptools/src/splib/string.inc"
#Include "../sptools/src/splib/file.inc"
#Include "../sptools/src/splib/vt100.inc"
#Include "../sptools/src/sptest/unittest.inc"

Const base% = Mm.Info(Option Base)

add_test("case sensitive ascending sort all",          "test_case_sens_ascending_sort_1")
add_test("case sensitive ascending sort beginning",    "test_case_sens_ascending_sort_2")
add_test("case sensitive ascending sort middle",       "test_case_sens_ascending_sort_3")
add_test("case sensitive ascending sort end",          "test_case_sens_ascending_sort_4")
add_test("case sensitive ascending sort single",       "test_case_sens_ascending_sort_5")
add_test("case sensitive ascending sort given empty smallest", "tst_case_sens_ascending_sort_6")
add_test("case sensitive ascending sort given empty biggest", "tst_case_sens_ascending_sort_7")
add_test("case sensitive descending sort all",         "test_case_sens_descending_sort_1")
add_test("case sensitive descending sort beginning",   "test_case_sens_descending_sort_2")
add_test("case sensitive descending sort middle",      "test_case_sens_descending_sort_3")
add_test("case sensitive descending sort end",         "test_case_sens_descending_sort_4")
add_test("case sensitive descending sort single",      "test_case_sens_descending_sort_5")
add_test("case sensitive descending sort given empty smallest", "tst_case_sens_descending_sort_6")
add_test("case sensitive descending sort given empty biggest", "tst_case_sens_descending_sort_7")
add_test("case insensitive ascending sort all",        "tst_case_insens_ascending_sort_1")
add_test("case insensitive ascending sort beginning",  "tst_case_insens_ascending_sort_2")
add_test("case insensitive ascending sort middle",     "tst_case_insens_ascending_sort_3")
add_test("case insensitive ascending sort end",        "tst_case_insens_ascending_sort_4")
add_test("case insensitive ascending sort single",     "tst_case_insens_ascending_sort_5")
add_test("case insensitive ascending sort given empty smallest", "tst_case_ins_ascending_sort_6")
add_test("case insensitive ascending sort given empty biggest", "tst_case_ins_ascending_sort_7")
add_test("case insensitive descending sort all",       "tst_case_ins_descending_sort_1")
add_test("case insensitive descending sort beginning", "tst_case_ins_descending_sort_2")
add_test("case insensitive descending sort middle",    "tst_case_ins_descending_sort_3")
add_test("case insensitive descending sort end",       "tst_case_ins_descending_sort_4")
add_test("case insensitive descending sort single",    "tst_case_ins_descending_sort_5")
add_test("case insensitive descending sort given empty smallest", "tst_case_ins_descending_sort_6")
add_test("case insensitive descending sort given empty biggest", "tst_case_ins_descending_sort_7")

If InStr(Mm.CmdLine$, "--base") Then run_tests() Else run_tests("--base=1")

End

' Case sensitive ascending sort of all elements.
Sub test_case_sens_ascending_sort_1()
  Local a$(array.new%(5)) = ("one", "two", "three", "four", "five")
  Local idx%(array.new%(5))

  Sort a$(), idx%()

  Local exp_a$(array.new%(5)) = ("five", "four", "one", "three", "two")
  Local exp_idx%(array.new%(5)) = (base% + 4, base% + 3, base% + 0, base% + 2, base% + 1)
  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case sensitive ascending sort of first 3 elements.
Sub test_case_sens_ascending_sort_2()
  Local a$(array.new%(5)) = ("one", "two", "three", "four", "five")
  Local idx%(array.new%(5))

  Sort a$(), idx%(), &b00, base%, 3

  Local exp_a$(array.new%(5)) = ("one", "three", "two", "four", "five")
  Local exp_idx%(array.new%(5)) = (base% + 0, base% + 2, base% + 1, base% + 3, base% + 4)
  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case sensitive ascending sort of middle 3 elements.
Sub test_case_sens_ascending_sort_3()
  Local a$(array.new%(5)) = ("one", "two", "three", "four", "five")
  Local idx%(array.new%(5))

  Sort a$(), idx%(), &b00, base% + 1, 3

  Local exp_a$(array.new%(5)) = ("one", "four", "three", "two", "five")
  Local exp_idx%(array.new%(5)) = (base% + 0, base% + 3, base% + 2, base% + 1, base% + 4)
  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case sensitive ascending sort of last 3 elements.
Sub test_case_sens_ascending_sort_4()
  Local a$(array.new%(5)) = ("one", "two", "three", "four", "five")
  Local idx%(array.new%(5))

  Sort a$(), idx%(), &b00, base% + 2, 3

  Local exp_a$(array.new%(5)) = ("one", "two", "five", "four", "three")
  Local exp_idx%(array.new%(5)) = (base% + 0, base% + 1, base% + 4, base% + 3, base% + 2)
  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case sensitive ascending sort of single elements.
Sub test_case_sens_ascending_sort_5()
  Local a$(array.new%(5)) = ("one", "two", "three", "four", "five")
  Local exp_a$(array.new%(5)) = ("one", "two", "three", "four", "five")
  Local exp_idx%(array.new%(5)) = (base% + 0, base% + 1, base% + 2, base% + 3, base% + 4)
  Local i%
  Local idx%(array.new%(5))

  For i% = base% To base% + 4
    Sort a$(), idx%(), &b00, i%, 1

    assert_string_array_equals(exp_a$(), a$())
    assert_int_array_equals(exp_idx%(), idx%())
  Next
End Sub

' Case sensitive ascending sort given empty is smallest value.
Sub tst_case_sens_ascending_sort_6()
  Local a$(array.new%(7)) = ("Ename", "Cname", "Aname", "bname", "Dname", "", "")
  Local exp_a$(array.new%(7)) = ("", "", "Aname", "Cname", "Dname", "Ename", "bname")
  Local exp_idx%(array.new%(7)) = (base% + 5, base% + 6, base% + 2, base% + 1, base% + 4, base% + 0, base% + 3)
  Local i%
  Local idx%(array.new%(7))

  Sort a$(), idx%(), &b000

  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case sensitive ascending sort given empty is biggest value.
Sub tst_case_sens_ascending_sort_7()
  Local a$(array.new%(7)) = ("Ename", "Cname", "Aname", "bname", "Dname", "", "")
  Local exp_a$(array.new%(7)) = ("Aname", "Cname", "Dname", "Ename", "bname", "", "")
  Local exp_idx%(array.new%(7)) = (base% + 2, base% + 1, base% + 4, base% + 0, base% + 3, base% + 5, base% + 6)
  Local i%
  Local idx%(array.new%(7))

  Sort a$(), idx%(), &b100

  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case sensitive decending sort of all elements.
Sub test_case_sens_descending_sort_1()
  Local a$(array.new%(5)) = ("one", "two", "three", "four", "five")
  Local idx%(array.new%(5))

  Sort a$(), idx%(), &b01

  Local exp_a$(array.new%(5)) = ("two", "three", "one", "four", "five")
  Local exp_idx%(array.new%(5)) = (base% + 1, base% + 2, base% + 0, base% + 3, base% + 4)
  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case sensitive descending sort of first 3 elements
Sub test_case_sens_descending_sort_2()
  Local a$(array.new%(5)) = ("one", "two", "three", "four", "five")
  Local idx%(array.new%(5))

  Sort a$(), idx%(), &b01, base%, 3

  Local exp_a$(array.new%(5)) = ("two", "three", "one", "four", "five")
  Local exp_idx%(array.new%(5)) = (base% + 1, base% + 2, base% + 0, base% + 3, base% + 4)
  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case sensitive descending sort of middle 3 elements.
Sub test_case_sens_descending_sort_3()
  Local a$(array.new%(5)) = ("one", "two", "three", "four", "five")
  Local idx%(array.new%(5))

  Sort a$(), idx%(), &b01, base% + 1, 3

  Local exp_a$(array.new%(5)) = ("one", "two", "three", "four", "five")
  Local exp_idx%(array.new%(5)) = (base% + 0, base% + 1, base% + 2, base% + 3, base% + 4)
  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case sensitive descending sort of last 3 elements.
Sub test_case_sens_descending_sort_4()
  Local a$(array.new%(5)) = ("one", "two", "three", "four", "five")
  Local idx%(array.new%(5))

  Sort a$(), idx%(), &b01, base% + 2, 3

  Local exp_a$(array.new%(5)) = ("one", "two", "three", "four", "five")
  Local exp_idx%(array.new%(5)) = (base% + 0, base% + 1, base% + 2, base% + 3, base% + 4)
  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case sensitive descending sort of single elements.
Sub test_case_sens_descending_sort_5()
  Local a$(array.new%(5)) = ("one", "two", "three", "four", "five")
  Local exp_a$(array.new%(5)) = ("one", "two", "three", "four", "five")
  Local exp_idx%(array.new%(5)) = (base% + 0, base% + 1, base% + 2, base% + 3, base% + 4)
  Local i%
  Local idx%(array.new%(5))

  For i% = base% To base% + 4
    Sort a$(), idx%(), &b01, i%, 1

    assert_string_array_equals(exp_a$(), a$())
    assert_int_array_equals(exp_idx%(), idx%())
  Next
End Sub

' Case sensitive descending sort given empty is smallest value.
Sub tst_case_sens_descending_sort_6()
  Local a$(array.new%(7)) = ("Ename", "Cname", "Aname", "bname", "Dname", "", "")
  Local exp_a$(array.new%(7)) = ("bname", "Ename", "Dname", "Cname", "Aname", "", "")
  Local exp_idx%(array.new%(7)) = (base% + 3, base% + 0, base% + 4, base% + 1, base% + 2, base% + 5, base% + 6)
  Local i%
  Local idx%(array.new%(7))

  Sort a$(), idx%(), &b001

  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case sensitive descending sort given empty is biggest value.
Sub tst_case_sens_descending_sort_7()
  Local a$(array.new%(7)) = ("Ename", "Cname", "Aname", "bname", "Dname", "", "")
  Local exp_a$(array.new%(7)) = ("", "", "bname", "Ename", "Dname", "Cname", "Aname")
  Local exp_idx%(array.new%(7)) = (base% + 5, base% + 6, base% + 3, base% + 0, base% + 4, base% + 1, base% + 2)
  Local i%
  Local idx%(array.new%(7))

  Sort a$(), idx%(), &b101

  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case insensitive ascending sort of all elements.
Sub tst_case_insens_ascending_sort_1()
  Local a$(array.new%(5)) = ("fubar1", "fuBar3", "Fubar2", "FuBar5", "FUBAR4")
  Local idx%(array.new%(5))

  Sort a$(), idx%(), &b10

  Local exp_a$(array.new%(5)) = ("fubar1", "Fubar2", "fuBar3", "FUBAR4", "FuBar5")
  Local exp_idx%(array.new%(5)) = (base% + 0, base% + 2, base% + 1, base% + 4, base% + 3)
  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case insensitive ascending sort of first 3 elements.
Sub tst_case_insens_ascending_sort_2()
  Local a$(array.new%(5)) = ("fubar1", "fuBar3", "Fubar2", "FuBar5", "FUBAR4")
  Local idx%(array.new%(5))

  Sort a$(), idx%(), &b10, base%, 3

  Local exp_a$(array.new%(5)) = ("fubar1", "Fubar2", "fuBar3", "FuBar5", "FUBAR4")
  Local exp_idx%(array.new%(5)) = (base% + 0, base% + 2, base% + 1, base% + 3, base% + 4)
  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case insensitive ascending sort of middle 3 elements.
Sub tst_case_insens_ascending_sort_3()
  Local a$(array.new%(5)) = ("fubar1", "fuBar3", "Fubar2", "FuBar5", "FUBAR4")
  Local idx%(array.new%(5))

  Sort a$(), idx%(), &b10, base% + 1, 3

  Local exp_a$(array.new%(5)) = ("fubar1", "Fubar2", "fuBar3", "FuBar5", "FUBAR4")
  Local exp_idx%(array.new%(5)) = (base% + 0, base% + 2, base% + 1, base% + 3, base% + 4)
  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case insensitive ascending sort of last 3 elements.
Sub tst_case_insens_ascending_sort_4()
  Local a$(array.new%(5)) = ("fubar1", "fuBar3", "Fubar2", "FuBar5", "FUBAR4")
  Local idx%(array.new%(5))

  Sort a$(), idx%(), &b10, base% + 2, 3

  Local exp_a$(array.new%(5)) = ("fubar1", "fuBar3", "Fubar2", "FUBAR4", "FuBar5")
  Local exp_idx%(array.new%(5)) = (base% + 0, base% + 1, base% + 2, base% + 4, base% + 3)
  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case insensitive ascending sort of single elements.
Sub tst_case_insens_ascending_sort_5()
  Local a$(array.new%(5)) = ("fubar1", "fuBar3", "Fubar2", "FuBar5", "FUBAR4")
  Local exp_a$(array.new%(5)) = ("fubar1", "fuBar3", "Fubar2", "FuBar5", "FUBAR4")
  Local exp_idx%(array.new%(5)) = (base% + 0, base% + 1, base% + 2, base% + 3, base% + 4)
  Local i%
  Local idx%(array.new%(5))

  For i% = base% To base% + 4
    Sort a$(), idx%(), &b10, i%, 1

    assert_string_array_equals(exp_a$(), a$())
    assert_int_array_equals(exp_idx%(), idx%())
  Next
End Sub

' Case insensitive ascending sort given empty is smallest value.
Sub tst_case_ins_ascending_sort_6()
  Local a$(array.new%(7)) = ("Ename", "Cname", "Aname", "bname", "Dname", "", "")
  Local exp_a$(array.new%(7)) = ("", "", "Aname", "bname", "Cname", "Dname", "Ename")
  Local exp_idx%(array.new%(7)) = (base% + 5, base% + 6, base% + 2, base% + 3, base% + 1, base% + 4, base% + 0)
  Local i%
  Local idx%(array.new%(7))

  Sort a$(), idx%(), &b010

  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case insensitive ascending sort given empty is biggest value.
Sub tst_case_ins_ascending_sort_7()
  Local a$(array.new%(7)) = ("Ename", "Cname", "Aname", "bname", "Dname", "", "")
  Local exp_a$(array.new%(7)) = ("Aname", "bname", "Cname", "Dname", "Ename", "", "")
  Local exp_idx%(array.new%(7)) = (base% + 2, base% + 3, base% + 1, base% + 4, base% + 0, base% + 5, base% + 6)
  Local i%
  Local idx%(array.new%(7))

  Sort a$(), idx%(), &b110

  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case insensitive descending sort of all elements.
Sub tst_case_ins_descending_sort_1()
  Local a$(array.new%(5)) = ("fubar1", "fuBar3", "Fubar2", "FuBar5", "FUBAR4")
  Local idx%(array.new%(5))

  Sort a$(), idx%(), &b11

  Local exp_a$(array.new%(5)) = ("FuBar5", "FUBAR4", "fuBar3", "Fubar2", "fubar1")
  Local exp_idx%(array.new%(5)) = (base% + 3, base% + 4, base% + 1, base% + 2, base% + 0)
  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case insensitive descending sort of first 3 elements.
Sub tst_case_ins_descending_sort_2()
  Local a$(array.new%(5)) = ("fubar1", "fuBar3", "Fubar2", "FuBar5", "FUBAR4")
  Local idx%(array.new%(5))

  Sort a$(), idx%(), &b11, base%, 3

  Local exp_a$(array.new%(5)) = ("fuBar3", "Fubar2", "fubar1", "FuBar5", "FUBAR4")
  Local exp_idx%(array.new%(5)) = (base% + 1, base% + 2, base% + 0, base% + 3, base% + 4)
  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case insensitive descending sort of middle 3 elements.
Sub tst_case_ins_descending_sort_3()
  Local a$(array.new%(5)) = ("fubar1", "fuBar3", "Fubar2", "FuBar5", "FUBAR4")
  Local idx%(array.new%(5))

  Sort a$(), idx%(), &b11, base% + 1, 3

  Local exp_a$(array.new%(5)) = ("fubar1", "FuBar5", "fuBar3", "Fubar2", "FUBAR4")
  Local exp_idx%(array.new%(5)) = (base% + 0, base% + 3, base% + 1, base% + 2, base% + 4)
  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case insensitive descending sort of last 3 elements.
Sub tst_case_ins_descending_sort_4()
  Local a$(array.new%(5)) = ("fubar1", "fuBar3", "Fubar2", "FuBar5", "FUBAR4")
  Local idx%(array.new%(5))

  Sort a$(), idx%(), &b11, base% + 2, 3

  Local exp_a$(array.new%(5)) = ("fubar1", "fuBar3", "FuBar5", "FUBAR4", "Fubar2")
  Local exp_idx%(array.new%(5)) = (base% + 0, base% + 1, base% + 3, base% + 4, base% + 2)
  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case insensitive descending sort of single elements.
Sub tst_case_ins_descending_sort_5()
  Local a$(array.new%(5)) = ("fubar1", "fuBar3", "Fubar2", "FuBar5", "FUBAR4")
  Local exp_a$(array.new%(5)) = ("fubar1", "fuBar3", "Fubar2", "FuBar5", "FUBAR4")
  Local exp_idx%(array.new%(5)) = (base% + 0, base% + 1, base% + 2, base% + 3, base% + 4)
  Local i%
  Local idx%(array.new%(5))

  For i% = base% To base% + 4
    Sort a$(), idx%(), &b11, i%, 1

    assert_string_array_equals(exp_a$(), a$())
    assert_int_array_equals(exp_idx%(), idx%())
  Next
End Sub

' Case insensitive descending sort given empty is smallest value.
Sub tst_case_ins_descending_sort_6()
  Local a$(array.new%(7)) = ("Ename", "Cname", "Aname", "bname", "Dname", "", "")
  Local exp_a$(array.new%(7)) = ("Ename", "Dname", "Cname", "bname", "Aname", "", "")
  Local exp_idx%(array.new%(7)) = (base% + 0, base% + 4, base% + 1, base% + 3, base% + 2, base% + 5, base% + 6)
  Local i%
  Local idx%(array.new%(7))

  Sort a$(), idx%(), &b011

  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub

' Case insensitive descending sort given empty is biggest value.
Sub tst_case_ins_descending_sort_7()
  Local a$(array.new%(7)) = ("Ename", "Cname", "Aname", "bname", "Dname", "", "")
  Local exp_a$(array.new%(7)) = ("", "", "Ename", "Dname", "Cname", "bname", "Aname")
  Local exp_idx%(array.new%(7)) = (base% + 5, base% + 6, base% + 0, base% + 4, base% + 1, base% + 3, base% + 2)
  Local i%
  Local idx%(array.new%(7))

  Sort a$(), idx%(), &b111

  assert_string_array_equals(exp_a$(), a$())
  assert_int_array_equals(exp_idx%(), idx%())
End Sub
