/****************/
// Mã sinh viên: 202419016
// Họ tên: Phạm Xuân Việt
/****************/
#ifndef PAYROLL_HPP
#define PAYROLL_HPP

#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace payroll {

// Lớp trừu tượng: giữ dữ liệu chung, không có công thức lương chung.
class Employee {
private:
    std::string employeeId_;
    std::string fullName_;
    std::string department_;
    double monthlyBonus_ = 0.0;

protected:
    void displayCommon(std::ostream& out) const;

public:
    Employee(std::string employeeId, std::string fullName);
    Employee(std::string employeeId, std::string fullName,
             std::string department);
    virtual ~Employee() = default;
    // Không cho gán/copy danh tính sau khi nhân sự vào bảng lương.
    Employee(const Employee&) = delete;
    Employee& operator=(const Employee&) = delete;

    const std::string& getEmployeeId() const noexcept;
    const std::string& getFullName() const noexcept;
    const std::string& getDepartment() const noexcept;
    double getMonthlyBonus() const noexcept;

    void addBonus(double amount);
    void addBonus(double amount, const std::string& reason);
    void addBonus(double rate, double referenceAmount,
                  const std::string& reason);
    void resetBonus() noexcept;

    virtual double calculateGrossPay() const = 0;
    virtual std::string getEmployeeType() const = 0;
    virtual void displayPayrollInfo(std::ostream& out) const = 0;
};

class SalariedEmployee final : public Employee {
private:
    double monthlySalary_;
    double responsibilityAllowance_;
public:
    SalariedEmployee(std::string id, std::string name, double salary);
    SalariedEmployee(std::string id, std::string name, std::string department,
                     double salary, double allowance);
    double calculateGrossPay() const override;
    std::string getEmployeeType() const override;
    void displayPayrollInfo(std::ostream& out) const override;
};

class HourlyEmployee final : public Employee {
private:
    double hourlyRate_;
    double workedHours_;
public:
    HourlyEmployee(std::string id, std::string name, double rate, double hours);
    HourlyEmployee(std::string id, std::string name, std::string department,
                   double rate, double hours);
    double calculateGrossPay() const override;
    std::string getEmployeeType() const override;
    void displayPayrollInfo(std::ostream& out) const override;
};

class SalesEmployee final : public Employee {
private:
    double baseSalary_;
    double salesRevenue_;
    double commissionRate_;
public:
    SalesEmployee(std::string id, std::string name, double baseSalary);
    SalesEmployee(std::string id, std::string name, std::string department,
                  double baseSalary, double revenue, double commissionRate);
    void setSalesRevenue(double revenue);
    double calculateGrossPay() const override;
    std::string getEmployeeType() const override;
    void displayPayrollInfo(std::ostream& out) const override;
};

// Payroll sở hữu các đối tượng đa hình bằng unique_ptr, không kế thừa Employee.
class Payroll {
private:
    std::string period_;
    std::vector<std::unique_ptr<Employee>> employees_;
public:
    explicit Payroll(std::string period);
    const std::string& getPeriod() const noexcept;
    std::size_t size() const noexcept;
    void addEmployee(std::unique_ptr<Employee> employee);
    Employee* findEmployee(const std::string& id) noexcept;
    const Employee* findEmployee(const std::string& id) const noexcept;
    double calculateTotalPayroll() const;
    double calculatePayrollByDepartment(const std::string& department) const;
    const Employee* findHighestPaidEmployee() const;
    void displayPayroll(std::ostream& out) const;
};
} // namespace payroll
#endif
