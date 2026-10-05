/****************/
// Mã sinh viên: 202419016
// Họ tên: Phạm Xuân Việt
/****************/
#include "payroll.hpp"
#include <exception>
#include <iomanip>
#include <iostream>
#include <memory>
#include <utility>

int main() {
    using namespace payroll;
    try {
        Payroll payroll("2026-09");
        auto salaried = std::make_unique<SalariedEmployee>(
            "E001", "Nguyễn Minh An", "Đào tạo", 15000000, 2000000);
        salaried->addBonus(1000000); // Nạp chồng một tham số.
        payroll.addEmployee(std::move(salaried));

        auto hourly = std::make_unique<HourlyEmployee>(
            "E002", "Trần Thu Bình", "Hỗ trợ", 100000, 150);
        hourly->addBonus(500000, "Hoan thanh cong viec"); // Hai tham số.
        payroll.addEmployee(std::move(hourly));
        payroll.addEmployee(std::make_unique<HourlyEmployee>(
            "E003", "Lê Hoàng Chi", "Hỗ trợ", 100000, 170));

        auto sales = std::make_unique<SalesEmployee>(
            "E004", "Phạm Quốc Dũng", "Kinh doanh", 8000000, 200000000, 0.05);
        sales->addBonus(0.02, 50000000, "Vuot chi tieu"); // Ba tham số.
        payroll.addEmployee(std::move(sales));

        payroll.displayPayroll(std::cout);
        std::cout << std::fixed << std::setprecision(2)
                  << "TONG PHONG HO TRO: "
                  << payroll.calculatePayrollByDepartment("Hỗ trợ") << " VND\n";
        if (const Employee* best = payroll.findHighestPaidEmployee()) {
            std::cout << "CAO NHAT: " << best->getEmployeeId() << " | "
                      << best->getFullName() << " | "
                      << best->calculateGrossPay() << " VND\n";
        }
    } catch (const std::exception& error) {
        std::cerr << "Loi: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
