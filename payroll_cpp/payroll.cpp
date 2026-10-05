/****************/
// Mã sinh viên: 202419016
// Họ tên: Phạm Xuân Việt
/****************/
#include "payroll.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace payroll {
namespace {
// Kiểm tra tập trung để các constructor và phương thức dùng cùng quy tắc.
void requireText(const std::string& value, const char* field) {
    if (value.empty() || std::all_of(value.begin(), value.end(),
            [](unsigned char c) { return std::isspace(c) != 0; })) {
        throw std::invalid_argument(std::string(field) + " must not be blank");
    }
}
void requireRange(double value, double low, double high, const char* field) {
    if (!std::isfinite(value) || value < low || value > high) {
        throw std::invalid_argument(std::string(field) + " is out of range");
    }
}
void requireNonnegative(double value, const char* field) {
    if (!std::isfinite(value) || value < 0.0) {
        throw std::invalid_argument(std::string(field) + " must be finite and >= 0");
    }
}
void requirePositive(double value, const char* field) {
    if (!std::isfinite(value) || value <= 0.0) {
        throw std::invalid_argument(std::string(field) + " must be finite and > 0");
    }
}
double finiteResult(double value) {
    if (!std::isfinite(value)) {
        throw std::overflow_error("Monetary calculation overflow");
    }
    return value;
}
std::string money(double amount) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << amount;
    return out.str();
}
bool validPeriod(const std::string& period) {
    if (period.size() != 7 || period[4] != '-') return false;
    for (std::size_t i = 0; i < period.size(); ++i) {
        if (i != 4 && (period[i] < '0' || period[i] > '9')) return false;
    }
    const int month = (period[5] - '0') * 10 + period[6] - '0';
    return period.substr(0, 4) != "0000" && month >= 1 && month <= 12;
}
} // namespace

Employee::Employee(std::string id, std::string name)
    : Employee(std::move(id), std::move(name), "Unassigned") {}
Employee::Employee(std::string id, std::string name, std::string department)
    : employeeId_(std::move(id)), fullName_(std::move(name)),
      department_(std::move(department)) {
    requireText(employeeId_, "employeeId");
    requireText(fullName_, "fullName");
    requireText(department_, "department");
}
const std::string& Employee::getEmployeeId() const noexcept { return employeeId_; }
const std::string& Employee::getFullName() const noexcept { return fullName_; }
const std::string& Employee::getDepartment() const noexcept { return department_; }
double Employee::getMonthlyBonus() const noexcept { return monthlyBonus_; }

void Employee::addBonus(double amount) {
    requirePositive(amount, "amount");
    // Chỉ cập nhật sau khi kiểm tra; lỗi không làm thay đổi tổng thưởng.
    monthlyBonus_ = finiteResult(monthlyBonus_ + amount);
}
void Employee::addBonus(double amount, const std::string& reason) {
    requireText(reason, "reason");
    addBonus(amount);
}
void Employee::addBonus(double rate, double referenceAmount,
                        const std::string& reason) {
    requireRange(rate, 0.0, 0.5, "bonusRate");
    requirePositive(rate, "bonusRate");
    requirePositive(referenceAmount, "referenceAmount");
    requireText(reason, "reason");
    addBonus(finiteResult(rate * referenceAmount));
}
void Employee::resetBonus() noexcept { monthlyBonus_ = 0.0; }
void Employee::displayCommon(std::ostream& out) const {
    out << employeeId_ << " | " << fullName_ << " | " << department_
        << " | " << getEmployeeType() << '\n'
        << "  Thuong: " << money(monthlyBonus_) << " VND\n";
}

SalariedEmployee::SalariedEmployee(std::string id, std::string name, double salary)
    : SalariedEmployee(std::move(id), std::move(name), "Unassigned", salary, 0.0) {}
SalariedEmployee::SalariedEmployee(std::string id, std::string name,
        std::string department, double salary, double allowance)
    : Employee(std::move(id), std::move(name), std::move(department)),
      monthlySalary_(salary), responsibilityAllowance_(allowance) {
    requireNonnegative(monthlySalary_, "monthlySalary");
    requireNonnegative(responsibilityAllowance_, "responsibilityAllowance");
}
double SalariedEmployee::calculateGrossPay() const {
    return finiteResult(monthlySalary_ + responsibilityAllowance_ + getMonthlyBonus());
}
std::string SalariedEmployee::getEmployeeType() const { return "SalariedEmployee"; }
void SalariedEmployee::displayPayrollInfo(std::ostream& out) const {
    displayCommon(out);
    out << "  Luong thang: " << money(monthlySalary_)
        << " | Phu cap: " << money(responsibilityAllowance_)
        << " | Thu nhap: " << money(calculateGrossPay()) << " VND\n";
}

HourlyEmployee::HourlyEmployee(std::string id, std::string name,
        double rate, double hours)
    : HourlyEmployee(std::move(id), std::move(name), "Unassigned", rate, hours) {}
HourlyEmployee::HourlyEmployee(std::string id, std::string name,
        std::string department, double rate, double hours)
    : Employee(std::move(id), std::move(name), std::move(department)),
      hourlyRate_(rate), workedHours_(hours) {
    requireNonnegative(hourlyRate_, "hourlyRate");
    requireRange(workedHours_, 0.0, 250.0, "workedHours");
}
double HourlyEmployee::calculateGrossPay() const {
    const double regularHours = std::min(workedHours_, 160.0);
    const double overtimeHours = std::max(workedHours_ - 160.0, 0.0);
    return finiteResult(regularHours * hourlyRate_
        + overtimeHours * hourlyRate_ * 1.5 + getMonthlyBonus());
}
std::string HourlyEmployee::getEmployeeType() const { return "HourlyEmployee"; }
void HourlyEmployee::displayPayrollInfo(std::ostream& out) const {
    displayCommon(out);
    const double regularHours = std::min(workedHours_, 160.0);
    const double overtimeHours = std::max(workedHours_ - 160.0, 0.0);
    out << "  Don gia: " << money(hourlyRate_) << " | Gio thuong: " << regularHours
        << " | Gio them: " << overtimeHours << '\n'
        << "  Tien gio thuong: " << money(regularHours * hourlyRate_)
        << " | Tien gio them: " << money(overtimeHours * hourlyRate_ * 1.5)
        << " | Thu nhap: " << money(calculateGrossPay()) << " VND\n";
}

SalesEmployee::SalesEmployee(std::string id, std::string name, double baseSalary)
    : SalesEmployee(std::move(id), std::move(name), "Unassigned", baseSalary, 0.0, 0.0) {}
SalesEmployee::SalesEmployee(std::string id, std::string name,
        std::string department, double baseSalary, double revenue, double commissionRate)
    : Employee(std::move(id), std::move(name), std::move(department)),
      baseSalary_(baseSalary), salesRevenue_(revenue), commissionRate_(commissionRate) {
    requireNonnegative(baseSalary_, "baseSalary");
    requireNonnegative(salesRevenue_, "salesRevenue");
    requireRange(commissionRate_, 0.0, 0.3, "commissionRate");
}
void SalesEmployee::setSalesRevenue(double revenue) {
    requireNonnegative(revenue, "salesRevenue");
    salesRevenue_ = revenue;
}
double SalesEmployee::calculateGrossPay() const {
    return finiteResult(baseSalary_ + salesRevenue_ * commissionRate_ + getMonthlyBonus());
}
std::string SalesEmployee::getEmployeeType() const { return "SalesEmployee"; }
void SalesEmployee::displayPayrollInfo(std::ostream& out) const {
    displayCommon(out);
    out << "  Luong co ban: " << money(baseSalary_) << " | Doanh so: " << money(salesRevenue_)
        << " | Ty le: " << commissionRate_ * 100.0 << "%\n"
        << "  Hoa hong: " << money(salesRevenue_ * commissionRate_)
        << " | Thu nhap: " << money(calculateGrossPay()) << " VND\n";
}

Payroll::Payroll(std::string period) : period_(std::move(period)) {
    if (!validPeriod(period_)) throw std::invalid_argument("period must be YYYY-MM");
}
const std::string& Payroll::getPeriod() const noexcept { return period_; }
std::size_t Payroll::size() const noexcept { return employees_.size(); }
void Payroll::addEmployee(std::unique_ptr<Employee> employee) {
    if (!employee) throw std::invalid_argument("employee must not be null");
    if (findEmployee(employee->getEmployeeId())) {
        throw std::invalid_argument("Duplicate employeeId");
    }
    employees_.push_back(std::move(employee));
}
Employee* Payroll::findEmployee(const std::string& id) noexcept {
    for (const auto& employee : employees_)
        if (employee->getEmployeeId() == id) return employee.get();
    return nullptr;
}
const Employee* Payroll::findEmployee(const std::string& id) const noexcept {
    for (const auto& employee : employees_)
        if (employee->getEmployeeId() == id) return employee.get();
    return nullptr;
}
double Payroll::calculateTotalPayroll() const {
    double total = 0.0;
    for (const auto& employee : employees_)
        total = finiteResult(total + employee->calculateGrossPay());
    return total;
}
double Payroll::calculatePayrollByDepartment(const std::string& department) const {
    requireText(department, "department");
    double total = 0.0;
    for (const auto& employee : employees_)
        if (employee->getDepartment() == department)
            total = finiteResult(total + employee->calculateGrossPay());
    return total;
}
const Employee* Payroll::findHighestPaidEmployee() const {
    const Employee* best = nullptr;
    for (const auto& employee : employees_) {
        // Giữ người thêm trước nếu đồng hạng.
        if (!best || employee->calculateGrossPay() > best->calculateGrossPay())
            best = employee.get();
    }
    return best;
}
void Payroll::displayPayroll(std::ostream& out) const {
    out << "BANG LUONG KY " << period_ << '\n';
    if (employees_.empty()) out << "Danh sach nhan su rong.\n";
    for (const auto& employee : employees_) employee->displayPayrollInfo(out);
    out << "TONG BANG LUONG: " << money(calculateTotalPayroll()) << " VND\n";
}
} // namespace payroll
