/****************/
// Mã sinh viên: 202419016
// Họ tên: Phạm Xuân Việt
/****************/
#include "payroll.hpp"
#include <cmath>
#include <exception>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
using namespace payroll;

namespace {
void check(bool condition) {
    if (!condition) throw std::runtime_error("Check failed");
}
// So sánh gần đúng cho double, không dùng assert bị tắt trong Release.
void near(double actual, double expected) {
    if (!std::isfinite(actual) || std::abs(actual-expected)>1e-6) {
        std::ostringstream msg;
        msg << "expected=" << expected << ", actual=" << actual;
        throw std::runtime_error(msg.str());
    }
}
template<class F> void invalid(F action) {
    try { action(); } catch (const std::invalid_argument&) { return; }
    throw std::runtime_error("Expected invalid_argument");
}
template<class F> void overflow(F action) {
    try { action(); } catch (const std::overflow_error&) { return; }
    throw std::runtime_error("Expected overflow_error");
}
Payroll sample() {
    Payroll p("2026-09");
    auto a=std::make_unique<SalariedEmployee>("E001","Nguyễn Minh An","Đào tạo",15000000,2000000);
    a->addBonus(1000000); p.addEmployee(std::move(a));
    auto b=std::make_unique<HourlyEmployee>("E002","Trần Thu Bình","Hỗ trợ",100000,150);
    b->addBonus(500000,"Dat yeu cau"); p.addEmployee(std::move(b));
    p.addEmployee(std::make_unique<HourlyEmployee>("E003","Lê Hoàng Chi","Hỗ trợ",100000,170));
    auto c=std::make_unique<SalesEmployee>("E004","Phạm Quốc Dũng","Kinh doanh",8000000,200000000,.05);
    c->addBonus(.02,50000000,"Vuot chi tieu"); p.addEmployee(std::move(c));
    return p;
}
struct TestCase { std::string id; std::string name; std::function<void()> run; };
} // namespace
int main() {
    const std::vector<TestCase> tests = {
        {"T01", "Nhân viên lương cố định theo đề", [] {
            SalariedEmployee e("E001", "Nguyễn Minh An", "Đào tạo", 15000000, 2000000); e.addBonus(1000000); near(e.calculateGrossPay(), 18000000);
        }},
        {"T02", "Nhân viên 150 giờ theo đề", [] {
            HourlyEmployee e("E002", "Trần Thu Bình", "Hỗ trợ", 100000, 150); e.addBonus(500000, "Dat yeu cau"); near(e.calculateGrossPay(), 15500000);
        }},
        {"T03", "Nhân viên 170 giờ theo đề", [] {
            HourlyEmployee e("E003", "Lê Hoàng Chi", "Hỗ trợ", 100000, 170); near(e.calculateGrossPay(), 17500000);
        }},
        {"T04", "Nhân viên kinh doanh theo đề", [] {
            SalesEmployee e("E004", "Phạm Quốc Dũng", "Kinh doanh", 8000000, 200000000, .05); e.addBonus(.02, 50000000, "Vuot chi tieu"); near(e.calculateGrossPay(), 19000000);
        }},
        {"T05", "Tổng bảng lương đa hình", [] {
            auto p = sample(); near(p.calculateTotalPayroll(), 70000000);
        }},
        {"T06", "Tổng phòng Hỗ trợ", [] {
            auto p = sample(); near(p.calculatePayrollByDepartment("Hỗ trợ"), 33000000);
        }},
        {"T07", "Người thu nhập cao nhất", [] {
            auto p = sample(); check(p.findHighestPaidEmployee()->getEmployeeId() == "E004");
        }},
        {"T08", "Tra cứu mã tồn tại và vắng mặt", [] {
            auto p = sample(); check(p.findEmployee("E002")->getFullName() == "Trần Thu Bình"); check(p.findEmployee("NO") == nullptr); const Payroll& cp = p; check(cp.findEmployee("E002") != nullptr); check(cp.findEmployee("NO") == nullptr);
        }},
        {"T09", "Giờ làm tại biên 0 và 160", [] {
            HourlyEmployee a("A", "A", 100000, 0), b("B", "B", 100000, 160); near(a.calculateGrossPay(), 0); near(b.calculateGrossPay(), 16000000);
        }},
        {"T10", "Giờ làm tại biên 250", [] {
            HourlyEmployee e("A", "A", 100000, 250); near(e.calculateGrossPay(), 29500000);
        }},
        {"T11", "Giờ lẻ ngay trên 160", [] {
            HourlyEmployee e("A", "A", 100000, 160.5); near(e.calculateGrossPay(), 16075000);
        }},
        {"T12", "Giờ âm và vượt 250", [] {
            invalid([]{ HourlyEmployee e("A", "A", 1, -.01); }); invalid([]{ HourlyEmployee e("A", "A", 1, 250.01); });
        }},
        {"T13", "Đơn giá giờ âm", [] {
            invalid([]{ HourlyEmployee e("A", "A", -1, 100); });
        }},
        {"T14", "Lương và phụ cấp âm", [] {
            invalid([]{ SalariedEmployee e("A", "A", -1); }); invalid([]{ SalariedEmployee e("A", "A", "D", 1, -1); });
        }},
        {"T15", "Lương cơ bản hoặc doanh số âm", [] {
            invalid([]{ SalesEmployee e("A", "A", -1); }); invalid([]{ SalesEmployee e("A", "A", "D", 1, -1, .1); });
        }},
        {"T16", "Hoa hồng tại biên 0 và 0,3", [] {
            SalesEmployee a("A", "A", "D", 0, 1000, 0), b("B", "B", "D", 0, 1000, .3); near(a.calculateGrossPay(), 0); near(b.calculateGrossPay(), 300);
        }},
        {"T17", "Hoa hồng âm và lớn hơn 0,3", [] {
            invalid([]{ SalesEmployee e("A", "A", "D", 1, 1, -.01); }); invalid([]{ SalesEmployee e("A", "A", "D", 1, 1, .3001); });
        }},
        {"T18", "Mã rỗng và chỉ có khoảng trắng", [] {
            invalid([]{ SalariedEmployee e("", "A", 1); }); invalid([]{ SalariedEmployee e(" 	", "A", 1); });
        }},
        {"T19", "Tên hoặc phòng ban rỗng", [] {
            invalid([]{ SalariedEmployee e("A", "", 1); }); invalid([]{ SalariedEmployee e("A", "A", " ", 1, 0); });
        }},
        {"T20", "Constructor rút gọn của ba lớp", [] {
            SalariedEmployee a("A", "A", 10); HourlyEmployee b("B", "B", 2, 3); SalesEmployee c("C", "C", 20); for (Employee* e : std::vector<Employee*>{&a,&b,&c}) { check(e->getDepartment() == "Unassigned"); near(e->getMonthlyBonus(), 0); } near(a.calculateGrossPay(), 10); near(b.calculateGrossPay(), 6); near(c.calculateGrossPay(), 20);
        }},
        {"T21", "Cộng dồn cả ba addBonus", [] {
            SalariedEmployee e("A", "A", 0); e.addBonus(100); e.addBonus(200, "A"); e.addBonus(.1, 1000, "B"); near(e.getMonthlyBonus(), 400);
        }},
        {"T22", "Thưởng cố định bằng 0 hoặc âm", [] {
            SalariedEmployee e("A", "A", 0); e.addBonus(10); invalid([&]{e.addBonus(0);}); invalid([&]{e.addBonus(-1, "A");}); near(e.getMonthlyBonus(), 10);
        }},
        {"T23", "Lý do rỗng ở hai phiên bản", [] {
            SalariedEmployee e("A", "A", 0); invalid([&]{e.addBonus(1, "");}); invalid([&]{e.addBonus(.1, 10, " 	");}); near(e.getMonthlyBonus(), 0);
        }},
        {"T24", "Tỷ lệ thưởng tại biên 0,5", [] {
            SalariedEmployee e("A", "A", 0); e.addBonus(.5, 1000, "A"); near(e.getMonthlyBonus(), 500);
        }},
        {"T25", "Tỷ lệ thưởng 0, âm hoặc trên 0,5", [] {
            SalariedEmployee e("A", "A", 0); for (double r : {0., -.1, .5001}) invalid([&]{ e.addBonus(r, 100, "A"); }); near(e.getMonthlyBonus(), 0);
        }},
        {"T26", "Giá trị tham chiếu 0 hoặc âm", [] {
            SalariedEmployee e("A", "A", 0); invalid([&]{e.addBonus(.1, 0, "A");}); invalid([&]{e.addBonus(.1, -1, "A");});
        }},
        {"T27", "Đặt lại thưởng", [] {
            SalariedEmployee e("A", "A", 100); e.addBonus(20); e.resetBonus(); near(e.getMonthlyBonus(), 0); near(e.calculateGrossPay(), 100);
        }},
        {"T28", "Cập nhật doanh số hợp lệ", [] {
            SalesEmployee e("A", "A", "D", 8000000, 200000000, .05); e.setSalesRevenue(240000000); near(e.calculateGrossPay(), 20000000);
        }},
        {"T29", "Cập nhật doanh số âm", [] {
            SalesEmployee e("A", "A", "D", 100, 1000, .05); invalid([&]{e.setSalesRevenue(-1);}); near(e.calculateGrossPay(), 150);
        }},
        {"T30", "Thêm trùng mã khác loại", [] {
            Payroll p("2026-09"); p.addEmployee(std::make_unique<SalariedEmployee>("A", "A", 100)); invalid([&]{p.addEmployee(std::make_unique<HourlyEmployee>("A", "B", 2, 3));}); check(p.size()==1); near(p.calculateTotalPayroll(),100);
        }},
        {"T31", "Thêm con trỏ null", [] {
            Payroll p("2026-09"); invalid([&]{p.addEmployee(nullptr);}); check(p.size()==0);
        }},
        {"T32", "Bảng lương rỗng", [] {
            Payroll p("2026-09"); near(p.calculateTotalPayroll(), 0); near(p.calculatePayrollByDepartment("D"), 0); check(p.findHighestPaidEmployee()==nullptr); std::ostringstream out; p.displayPayroll(out); check(out.str().find("rong")!=std::string::npos);
        }},
        {"T33", "Phòng không tồn tại và phòng rỗng", [] {
            auto p=sample(); near(p.calculatePayrollByDepartment("Khac"),0); invalid([&]{p.calculatePayrollByDepartment(" ");});
        }},
        {"T34", "Hai người đồng hạng", [] {
            Payroll p("2026-09"); p.addEmployee(std::make_unique<SalariedEmployee>("A","A",100)); p.addEmployee(std::make_unique<HourlyEmployee>("B","B",10,10)); check(p.findHighestPaidEmployee()->getEmployeeId()=="A");
        }},
        {"T35", "Định dạng kỳ lương", [] {
            Payroll a("2026-01"), b("2026-12"); check(a.getPeriod()=="2026-01"); for (const char* s : {"", "2026-00", "2026-13", "2026-9", "20x6-09", "0000-09"}) invalid([&]{Payroll p(s);});
        }},
        {"T36", "NaN và vô cực trong dữ liệu số", [] {
            const double nan=std::numeric_limits<double>::quiet_NaN(), inf=std::numeric_limits<double>::infinity(); invalid([&]{SalariedEmployee e("A","A",nan);}); invalid([&]{HourlyEmployee e("A","A",1,inf);}); invalid([&]{SalesEmployee e("A","A","D",0,0,nan);}); SalariedEmployee e("A","A",0); invalid([&]{e.addBonus(inf);}); invalid([&]{e.addBonus(nan,100,"A");}); SalesEmployee s("S","S",0); invalid([&]{s.setSalesRevenue(inf);});
        }},
        {"T37", "Tràn tổng thưởng", [] {
            SalariedEmployee e("A","A",0); const double m=std::numeric_limits<double>::max(); e.addBonus(m); overflow([&]{e.addBonus(m);}); check(e.getMonthlyBonus()==m);
        }},
        {"T38", "Tràn thu nhập và tổng bảng lương", [] {
            const double m=std::numeric_limits<double>::max(); SalariedEmployee e("A","A","D",m,m); overflow([&]{e.calculateGrossPay();}); Payroll p("2026-09"); p.addEmployee(std::make_unique<SalariedEmployee>("A","A",m)); p.addEmployee(std::make_unique<SalariedEmployee>("B","B",m)); overflow([&]{p.calculateTotalPayroll();});
        }},
        {"T39", "Hiển thị đa hình và các thành phần", [] {
            auto p=sample(); std::ostringstream out; p.displayPayroll(out); for (const char* token : {"SalariedEmployee", "HourlyEmployee", "SalesEmployee", "Phu cap:", "Gio them: 10", "Hoa hong:", "70000000.00"}) check(out.str().find(token)!=std::string::npos);
        }},
        {"T40", "Thêm lớp mới không sửa Payroll", [] {
            class ContractEmployee final : public Employee { public: ContractEmployee():Employee("C","Contract"){} double calculateGrossPay() const override {return 1234 + getMonthlyBonus();} std::string getEmployeeType() const override {return "ContractEmployee";} void displayPayrollInfo(std::ostream& out) const override {out << getEmployeeType();} }; Payroll p("2026-09"); p.addEmployee(std::make_unique<ContractEmployee>()); near(p.calculateTotalPayroll(),1234); check(p.findHighestPaidEmployee()->getEmployeeType()=="ContractEmployee");
        }},
    };
    std::size_t passed=0;
    for (const auto& test:tests) {
        try {
            test.run(); ++passed;
            std::cout << "[PASS] " << test.id << " | " << test.name << '\n';
        } catch (const std::exception& e) {
            std::cout << "[FAIL] " << test.id << " | " << test.name << " | " << e.what() << '\n';
        }
    }
    std::cout << "RESULT: " << passed << "/" << tests.size() << " passed\n";
    return passed==tests.size()?0:1;
}
