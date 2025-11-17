import os
import sys

sys.path.append(os.path.dirname(os.path.abspath(__file__)))

try:
    import func_core

    print("[Python Wrapper] Successfully imported C++ func_core module.")
    PortScanResult = func_core.PortScanResult
    WhoisInfo = func_core.WhoisInfo
    DNSRecord = func_core.DNSRecord

except ImportError as e:
    print(f"Details: {e}")

    # --------------ports-------------------
    class PortScanResult:
        def __init__(self, port, status, service):
            self.port = port
            self.status = status
            self.service = service

    def execute_scan(target: str, ports: str, scan_type: str) -> list[PortScanResult]:
        return [
            PortScanResult(port=1, status="Error", service="C++ Core Missing"),
            PortScanResult(port=2, status="Error", service="Check Build Step"),
        ]

    # --------------whois-------------------
    class WhoisInfo:
        def __init__(
            self,
            domain=None,
            registryDomainID=None,
            registrar=None,
            registrarWhoisServer=None,
            registrarURL=None,
            creationDate=None,
            updatedDate=None,
            expiryDate=None,
            statuses=None,
            nameServers=None,
            dnssec=None,
        ):
            self.domain = domain
            self.registryDomainID = registryDomainID
            self.registrar = registrar
            self.registrarWhoisServer = registrarWhoisServer
            self.registrarURL = registrarURL
            self.creationDate = creationDate
            self.updatedDate = updatedDate
            self.expiryDate = expiryDate
            self.statuses = statuses or []
            self.nameServers = nameServers or []
            self.dnssec = dnssec

    def execute_whois(target: str) -> WhoisInfo:
        return WhoisInfo(
            domain=target,
            registryDomainID="Error",
            registrar="C++ Core Missing",
            registrarWhoisServer=None,
            registrarURL=None,
            creationDate=None,
            updatedDate=None,
            expiryDate=None,
            statuses=[],
            nameServers=[],
            dnssec=None,
        )

    # --------------dns_record-------------------

    class DNSRecord:
        def __init__(self, type, value, ttl):
            self.type = type
            self.value = value
            self.ttl = ttl

    def execute_dns_record(target: str) -> list[DNSRecord]:
        return [
            DNSRecord(ttl=1, type="Error", value="C++ Core Missing"),
            DNSRecord(ttl=2, type="Error", value="Check Build Step"),
        ]

    print("[Python Wrapper] Running in safe mock mode.")

if "func_core" in sys.modules:
    # --------------ports-------------------
    def execute_scan(target: str, ports: str, scan_type: str) -> list[PortScanResult]:
        results_cpp = func_core.execute_scan(target, ports, scan_type)
        return results_cpp

    # --------------whois-------------------
    def execute_whois(target: str) -> WhoisInfo:
        results_cpp = func_core.execute_whois(target)
        return results_cpp

    # --------------dns_record-------------------

    def execute_dns_record(target: str) -> list[DNSRecord]:
        results_cpp = func_core.execute_dns_record(target)
        return results_cpp
